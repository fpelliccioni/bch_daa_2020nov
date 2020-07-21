#!/usr/bin/env python3

import argparse
import datetime
import math
import random
import statistics
import sys
import time
from collections import namedtuple
from functools import partial
from operator import attrgetter
from threading import Lock

import aserti3416cpp

def bits_to_target(bits):
    size = bits >> 24
    assert size <= 0x1d

    word = bits & 0x00ffffff
    assert 0x8000 <= word <= 0x7fffff

    if size <= 3:
        return word >> (8 * (3 - size))
    else:
        return word << (8 * (size - 3))

MAX_BITS = 0x1d00ffff
MAX_TARGET = bits_to_target(MAX_BITS)

def target_to_bits(target):
    assert target > 0
    if target > MAX_TARGET:
        print('Warning: target went above maximum ({} > {})'
              .format(target, MAX_TARGET), file=sys.stderr)
        target = MAX_TARGET
    size = (target.bit_length() + 7) // 8
    mask64 = 0xffffffffffffffff
    if size <= 3:
        compact = (target & mask64) << (8 * (3 - size))
    else:
        compact = (target >> (8 * (size - 3))) & mask64

    if compact & 0x00800000:
        compact >>= 8
        size += 1

    assert compact == (compact & 0x007fffff)
    assert size < 256
    return compact | size << 24

def bits_to_work(bits):
    return (2 << 255) // (bits_to_target(bits) + 1)

def target_to_hex(target):
    h = hex(target)[2:]
    return '0' * (64 - len(h)) + h

TARGET_1 = bits_to_target(486604799)

default_params = {
    'INITIAL_BCC_BITS':0x18084bb7,
    'INITIAL_SWC_BITS':0x18013ce9,
    'INITIAL_FX':0.19,
    'INITIAL_TIMESTAMP':1503430225,
    'INITIAL_HASHRATE':1000,    # In PH/s.
    'INITIAL_HEIGHT':481824,
    'BTC_fees':0.02,
    'BCH_fees':0.002,
    'num_blocks':10000,

    # Steady hashrate mines the BCC chain all the time.  In PH/s.
    'STEADY_HASHRATE':300,

    # Variable hash is split across both chains according to relative
    # revenue.  If the revenue ratio for either chain is at least 15%
    # higher, everything switches.  Otherwise the proportion mining the
    # chain is linear between +- 15%.
    'VARIABLE_HASHRATE':2000,   # In PH/s.
    'VARIABLE_PCT':15,   # 85% to 115%
    'VARIABLE_WINDOW':6,  # No of blocks averaged to determine revenue ratio
    'MEMORY_GAIN':.01,            # if rev_ratio is 1.01, then the next block's HR will be 0.01*MEMORY_GAIN higher

    # Greedy hashrate switches chain if that chain is more profitable for
    # GREEDY_WINDOW BCC blocks.  It will only bother to switch if it has
    # consistently been GREEDY_PCT more profitable.
    'GREEDY_HASHRATE':2000,     # In PH/s.
    'GREEDY_PCT':10,
    'GREEDY_WINDOW':6,
}
IDEAL_BLOCK_TIME = 10 * 60

State = namedtuple('State', 'height wall_time timestamp bits chainwork fx '
                   'hashrate rev_ratio var_frac memory_frac greedy_frac msg')

states = []
lock = Lock() # hack to deal with concurrency and global use of states


def print_headers():
    print(', '.join(['Height', 'FX', 'Block Time', 'Unix', 'Timestamp',
                     'Difficulty (bn)', 'Implied Difficulty (bn)',
                     'Hashrate (PH/s)', 'Rev Ratio', 'memory_hashrate', 'Greedy?', 'Comments']))

def print_state():
    state = states[-1]
    block_time = state.timestamp - states[-2].timestamp
    t = datetime.datetime.fromtimestamp(state.timestamp)
    difficulty = TARGET_1 / bits_to_target(state.bits)
    implied_diff = TARGET_1 / ((2 << 255) / (state.hashrate * 1e15 * IDEAL_BLOCK_TIME))
    print(', '.join(['{:d}'.format(state.height),
                     '{:.8f}'.format(state.fx),
                     '{:d}'.format(block_time),
                     '{:d}'.format(state.timestamp),
                     '{:%Y-%m-%d %H:%M:%S}'.format(t),
                     '{:.2f}'.format(difficulty / 1e9),
                     '{:.2f}'.format(implied_diff / 1e9),
                     '{:.0f}'.format(state.hashrate),
                     '{:.3f}'.format(state.rev_ratio),
                     'Yes' if state.greedy_frac == 1.0 else 'No',
                     state.msg]))

def revenue_ratio(fx, BCC_target, params):
    '''Returns the instantaneous SWC revenue rate divided by the
    instantaneous BCC revenue rate.  A value less than 1.0 makes it
    attractive to mine BCC.  Greater than 1.0, SWC.'''
    SWC_fees = params['BTC_fees'] * random.random()
    SWC_revenue = 12.5 + SWC_fees
    SWC_target = bits_to_target(default_params['INITIAL_SWC_BITS'])

    BCC_fees = params['BCH_fees'] * random.random()
    BCC_revenue = (12.5 + BCC_fees) * fx

    SWC_difficulty_ratio = BCC_target / SWC_target
    return SWC_revenue / SWC_difficulty_ratio / BCC_revenue

def median_time_past(states):
    times = [state.timestamp for state in states]
    return sorted(times)[len(times) // 2]

def suitable_block_index(index):
    assert index >= 2
    indices = [index - 2, index - 1, index]

    if states[indices[0]].timestamp > states[indices[2]].timestamp:
        indices[0], indices[2] = indices[2], indices[0]

    if states[indices[0]].timestamp > states[indices[1]].timestamp:
        indices[0], indices[1] = indices[1], indices[0]

    if states[indices[1]].timestamp > states[indices[2]].timestamp:
        indices[1], indices[2] = indices[2], indices[1]

    return indices[1]



# void* CAPI_CBlockIndex_construct() {
# void CAPI_CBlockIndex_destruct(void* ptr) {
# void CAPI_CBlockIndex_set_nHeight(void* ptr, int nHeight) {
# int CAPI_CBlockIndex_get_nHeight(void* ptr) {
# void CAPI_CBlockIndex_set_nTime(void* ptr, uint32_t nTime) {
# void CAPI_CBlockIndex_set_nBits(void* ptr, uint32_t nBits) {
# void CAPI_CBlockIndex_set_nChainWork(void* ptr, void* nChainWork) {

def next_bits_aserti_416_cpp(msg, tau, mode=1, mo3=False):

    # const CBlockIndex *prefBlock = pindexPrev->GetAncestor(nRefHeight);
    # assert(prefBlock != nullptr);

    # const int64_t nTimeDiff = pindexPrev->nTime - prefBlock->GetBlockHeader().nTime;
    # const int32_t nHeightDiff = pindexPrev->nHeight - prefBlock->nHeight;
    # assert(nHeightDiff > 0);

    if mo3:
        last = suitable_block_index(len(states) - 1)
        first = suitable_block_index(2)
    else:
        last = len(states)-1
        first = 0

    blocks = []
    for i in range(first, last + 1):
        state = states[i]
        block = aserti3416cpp.CBlockIndex_construct()
        aserti3416cpp.CBlockIndex_set_nTime(block, state.timestamp)
        aserti3416cpp.CBlockIndex_set_nHeight(block, state.height)
        aserti3416cpp.CBlockIndex_set_nBits(block, state.bits)
        blocks.append(block)

    for i in range(len(blocks) - 1, 0, -1):
        aserti3416cpp.CBlockIndex_set_pprev(blocks[i], blocks[i-1])

    blkHeaderDummy = aserti3416cpp.CBlockHeader_construct()
    params = aserti3416cpp.Params_GetDefaultMainnetConsensusParams()
    # nforkHeight = states[first].height
    nforkHeight = states[0].height

    # res = aserti3416cpp.GetNextASERTWorkRequired(blocks[-1], blkHeaderDummy, params, nforkHeight)
    res = aserti3416cpp.GetNextASERTWorkRequired(blocks[-1], blkHeaderDummy, params, blocks[0], False)

    # aserti3416cpp.CBlockHeader_destruct(blkHeaderDummy)
    # aserti3416cpp.Params_destruct(params)
    # for block in blocks:
    #     aserti3416cpp.CBlockIndex_destruct(block)

    return res


def next_bits_aserti(msg, tau, mode=1, mo3=False):
    rbits = 16      # number of bits after the radix for fixed-point math
    radix = 1<<rbits
    if mo3:
        last = suitable_block_index(len(states) - 1)
        first = suitable_block_index(2)
    else:
        last = len(states)-1
        first = 0

    # print(f'rbits:                   {rbits}')
    # print(f'radix:                   {radix}')
    # print(f'IDEAL_BLOCK_TIME:        {IDEAL_BLOCK_TIME}')
    # print(f'tau:                     {tau}')

    # print(f'first:                   {first}')
    # print(f'last:                    {last}')
    # print(f'states[first].timestamp: {states[first].timestamp}')
    # print(f'states[first].height:    {states[first].height}')
    # print(f'states[last].timestamp:  {states[last].timestamp}')
    # print(f'states[last].height:     {states[last].height}')
    # print(f'states[0].bits:          {states[0].bits}')

    blocks_time = states[last].timestamp - states[first].timestamp
    height_diff = states[last].height    - states[first].height
    target = bits_to_target(states[0].bits)

    # print(f'blocks_time:              {blocks_time}')
    # print(f'height_diff:              {height_diff}')
    # print(f'target:                   {target}')
    # print(f'target_to_bits(target):   {target_to_bits(target)}')

    # Ultimately, we want to approximate the following ASERT formula, using only integer (fixed-point) math:
    #     new_target = old_target * 2^((blocks_time - IDEAL_BLOCK_TIME*(height_diff+1)) / tau)

    # First, we'll calculate the exponent:
    exponent = ((blocks_time - IDEAL_BLOCK_TIME*height_diff) * radix) // tau

    # print(f'exponent:                {exponent}')

    # Next, we use the 2^x = 2 * 2^(x-1) identity to shift our exponent into the (0, 1] interval.
    # First, the truncated exponent tells us how many shifts we need to do
    shifts = exponent >> rbits
    # print(f'shifts:              {shifts}')

    # Next, we shift. Python doesn't allow shifting by negative integers, so:
    if shifts < 0:
        # print("IF shifts < 0")
        target >>= -shifts
    else:
        # print("ELSE shifts < 0")
        target <<= shifts
    exponent -= shifts*radix

    # print(f'target:                   {target}')
    # print(f'target_to_bits(target):   {target_to_bits(target)}')
    # print(f'exponent:                {exponent}')

    # Now we compute an approximated target * 2^(exponent)
    if mode == 1:
        # target * 2^x ~= target * (1 + x)
        target += (target * exponent) >> rbits
    elif mode == 2:
        # target * 2^x ~= target * (1 + 2*x/3 + x**2/3)
        target += (target * 2*exponent*radix//3 + target*exponent*exponent //3) >> (rbits*2)
    elif mode == 3:
        # target * 2^x ~= target * (1 + 0.695502049*x + 0.2262698*x**2 + 0.0782318*x**3)
        factor = (195766423245049*exponent + 971821376*exponent**2 + 5127*exponent**3 + 2**47)>>(rbits*3)
        target += (target * factor) >> rbits

    # print(f'factor:                   {factor}')
    # print(f'target:                   {target}')
    # print(f'target_to_bits(target):   {target_to_bits(target)}')

    return target_to_bits(target)

def block_time(mean_time):
    # Sample the exponential distn
    sample = random.random()
    lmbda = 1 / mean_time
    return math.log(1 - sample) / -lmbda

def next_fx_random(r, **params):
    return states[-1].fx * (1.0 + (r - 0.5) / 200)

def next_fx_constant(r, **params):
    return states[-1].fx

def next_fx_ramp(r, **params):
    return states[-1].fx * 1.00017149454

def next_hashrate(states, scenario, params):
    msg = []
    high = 1.0 + params['VARIABLE_PCT'] / 100
    scale_fac = 50 / params['VARIABLE_PCT']
    N = params['VARIABLE_WINDOW']
    mean_rev_ratio = sum(state.rev_ratio for state in states[-N:]) / N

    if 1:
        var_fraction = (high - mean_rev_ratio**params['VARIABLE_EXPONENT']) * scale_fac
        memory_frac = states[-1].memory_frac +  ((var_fraction-.5) * params['MEMORY_GAIN'])
        var_fraction = max(0, min(1, var_fraction + memory_frac))
    else:
        var_fraction = (high - mean_rev_ratio**params['VARIABLE_EXPONENT']) * scale_fac
        memory_frac  = states[-1].memory_frac * 2**(1*(1-mean_rev_ratio))
        if var_fraction > 0 and memory_frac <= 0:
            memory_frac = var_fraction
        var_fraction = ((1-mean_rev_ratio**params['VARIABLE_EXPONENT'])*4 + states[-1].memory_frac)
        a = 0.2
        memory_frac = (1-a) * states[-1].memory_frac + a * var_fraction
        var_fraction = 2**(var_fraction*20)
        memory_frac = max(-20, min(0, memory_frac))
        var_fraction = max(0, min(1, var_fraction))
    

    if ((scenario.pump_144_threshold > 0) and
        (states[-1-144+5].timestamp - states[-1-144].timestamp > scenario.pump_144_threshold)):
        var_fraction = max(var_fraction, .25)


    # mem_rev_ratio = sum(state.rev_ratio for state in states[-params['MEMORY_WINDOW']:]) / params['MEMORY_WINDOW']
    # memdelta = (1-mem_rev_ratio**params['MEMORY_POWER'])*params['MEMORY_GAIN']
    # if params['MEMORY_REMAINING']:
    #     memory_frac = states[-1].memory_frac + memdelta*(1-states[-1].memory_frac)
    # else:
    #     memory_frac = states[-1].memory_frac + memdelta
    # memory_frac = max(0.0, min(1.0, memory_frac))

    N = params['GREEDY_WINDOW']
    gready_rev_ratio = sum(state.rev_ratio for state in states[-N:]) / N
    greedy_frac = states[-1].greedy_frac
    if mean_rev_ratio >= 1 + params['GREEDY_PCT'] / 100:
        if greedy_frac != 0.0:
            msg.append("Greedy miners left")
        greedy_frac = 0.0
    elif mean_rev_ratio <= 1 - params['GREEDY_PCT'] / 100:
        if greedy_frac != 1.0:
            msg.append("Greedy miners joined")
        greedy_frac = 1.0

    hashrate = (params['STEADY_HASHRATE'] + scenario.dr_hashrate
                + params['VARIABLE_HASHRATE'] * var_fraction
                #+ params['MEMORY_HASHRATE'] * memory_frac
                + params['GREEDY_HASHRATE'] * greedy_frac)

    return hashrate, msg, var_fraction, memory_frac, greedy_frac

def next_step(fx_jump_factor, params):
    algo, scenario = params['algo'], params['scenario']
    hashrate, msg, var_frac, memory_frac, greedy_frac = next_hashrate(states, scenario, params)
    # First figure out our hashrate
    # Calculate our dynamic difficulty
    bits = algo.next_bits(msg, **algo.params)
    target = bits_to_target(bits)
    print(f'bits: {bits}')
    # print(f'target: {target}')
    # See how long we take to mine a block
    mean_hashes = pow(2, 256) // target
    mean_time = mean_hashes / (hashrate * 1e15)
    time = int(block_time(mean_time) + 0.5)
    wall_time = states[-1].wall_time + time
    # Did the difficulty ramp hashrate get the block?
    if random.random() < (abs(scenario.dr_hashrate) / hashrate):
        if (scenario.dr_hashrate > 0):
            timestamp = median_time_past(states[-11:]) + 1
        else:
            timestamp = wall_time + 2 * 60 * 60
    else:
        timestamp = wall_time
    # Get a new FX rate
    rand = random.random()
    fx = scenario.next_fx(rand, **scenario.params)
    if fx_jump_factor != 1.0:
        msg.append('FX jumped by factor {:.2f}'.format(fx_jump_factor))
        fx *= fx_jump_factor
    rev_ratio = revenue_ratio(fx, target, params)

    chainwork = states[-1].chainwork + bits_to_work(bits)

    # add a state
    states.append(State(states[-1].height + 1, wall_time, timestamp,
                        bits, chainwork, fx, hashrate, rev_ratio,
                        var_frac, memory_frac, greedy_frac, ' / '.join(msg)))

Algo = namedtuple('Algo', 'next_bits params')

Algos = {

    'aserti1-144' : Algo(next_bits_aserti, {
        'tau': int(math.log(2) * IDEAL_BLOCK_TIME * 144),
        'mode': 1,
    }),
    'aserti1-288' : Algo(next_bits_aserti, {
        'tau': int(math.log(2) * IDEAL_BLOCK_TIME * 288),
        'mode': 1,
    }),
    'aserti1-576' : Algo(next_bits_aserti, {
        'tau': int(math.log(2) * IDEAL_BLOCK_TIME * 576),
        'mode': 1,
    }),

    'aserti2-144' : Algo(next_bits_aserti, {
        'tau': int(math.log(2) * IDEAL_BLOCK_TIME * 144),
        'mode': 2,
    }),
    'aserti2-288' : Algo(next_bits_aserti, {
        'tau': int(math.log(2) * IDEAL_BLOCK_TIME * 288),
        'mode': 2,
    }),
    'aserti2-576' : Algo(next_bits_aserti, {
        'tau': int(math.log(2) * IDEAL_BLOCK_TIME * 576),
        'mode': 2,
    }),

    'aserti3-072' : Algo(next_bits_aserti, {
        'tau': int(math.log(2) * IDEAL_BLOCK_TIME *  72),
        'mode': 3,
    }),
    'aserti3-144' : Algo(next_bits_aserti, {
        'tau': int(math.log(2) * IDEAL_BLOCK_TIME * 144),
        'mode': 3,
    }),
    'aserti3-200' : Algo(next_bits_aserti, {
        'tau': int(math.log(2) * IDEAL_BLOCK_TIME * 200),
        'mode': 3,
    }),
    'aserti3-208' : Algo(next_bits_aserti, {
        'tau': int(math.log(2) * IDEAL_BLOCK_TIME * 208),
        'mode': 3,
    }),
    'aserti3-288' : Algo(next_bits_aserti, {
        'tau': int(math.log(2) * IDEAL_BLOCK_TIME * 288),
        'mode': 3,
    }),
    'aserti3-416' : Algo(next_bits_aserti, {
        'tau': int(math.log(2) * IDEAL_BLOCK_TIME * 416),
        'mode': 3,
    }),
    'aserti3-576' : Algo(next_bits_aserti, {
        'tau': int(math.log(2) * IDEAL_BLOCK_TIME * 576),
        'mode': 3,
    }),


    'aserti3-mo3-072' : Algo(next_bits_aserti, {
        'tau': int(math.log(2) * IDEAL_BLOCK_TIME *  72),
        'mode': 3, 'mo3':True,
    }),
    'aserti3-mo3-144' : Algo(next_bits_aserti, {
        'tau': int(math.log(2) * IDEAL_BLOCK_TIME * 144),
        'mode': 3, 'mo3':True,
    }),
    'aserti3-mo3-200' : Algo(next_bits_aserti, {
        'tau': int(math.log(2) * IDEAL_BLOCK_TIME * 200),
        'mode': 3, 'mo3':True,
    }),
    'aserti3-mo3-208' : Algo(next_bits_aserti, {
        'tau': int(math.log(2) * IDEAL_BLOCK_TIME * 208),
        'mode': 3, 'mo3':True,
    }),
    'aserti3-mo3-288' : Algo(next_bits_aserti, {
        'tau': int(math.log(2) * IDEAL_BLOCK_TIME * 288),
        'mode': 3, 'mo3':True,
    }),
    'aserti3-mo3-416' : Algo(next_bits_aserti, {
        'tau': int(math.log(2) * IDEAL_BLOCK_TIME * 416),
        'mode': 3, 'mo3':True,
    }),
    'aserti3-mo3-576' : Algo(next_bits_aserti, {
        'tau': int(math.log(2) * IDEAL_BLOCK_TIME * 576),
        'mode': 3, 'mo3':True,
    }),

    # C++ wrappers
    'aserti3-416-cpp' : Algo(next_bits_aserti_416_cpp, {
        'tau': int(math.log(2) * IDEAL_BLOCK_TIME * 416),
        'mode': 3,
    }),
    'aserti3-mo3-416-cpp' : Algo(next_bits_aserti_416_cpp, {
        'tau': int(math.log(2) * IDEAL_BLOCK_TIME * 416),
        'mode': 3, 'mo3':True,
    }),
}

Scenario = namedtuple('Scenario', 'next_fx, params, dr_hashrate, pump_144_threshold')

Scenarios = {
    'default' : Scenario(next_fx_random, {}, 0, 0),
    'stable' : Scenario(next_fx_constant, {"price1x":True}, 0, 0),
    'fxramp' : Scenario(next_fx_ramp, {}, 0, 0),
    # Difficulty rampers with given PH/s
    'dr50' : Scenario(next_fx_random, {}, 50, 0),
    'dr75' : Scenario(next_fx_random, {}, 75, 0),
    'dr100' : Scenario(next_fx_random, {}, 100, 0),
    'pump-osc' : Scenario(next_fx_ramp, {}, 0, 8000),
    'ft50' : Scenario(next_fx_random, {}, -50, 0),
    'ft100' : Scenario(next_fx_random, {}, -100, 0),
    'price10x' : Scenario(next_fx_random, {"price10x":True}, 0, 0),
}

def run_one_simul(print_it, returnstate=False, params=default_params):
    lock.acquire()
    states.clear()

    try:
        # Initial state is afer 2020 steady prefix blocks
        N = 2020
        for n in range(-N, 0):
            state = State(params['INITIAL_HEIGHT'] + n, 
                          params['INITIAL_TIMESTAMP'] + n * IDEAL_BLOCK_TIME,
                          params['INITIAL_TIMESTAMP'] + n * IDEAL_BLOCK_TIME,
                          params['INITIAL_BCC_BITS'], 
                          bits_to_work(params['INITIAL_BCC_BITS']) * (n + N + 1),
                          params['INITIAL_FX'], 
                          params['INITIAL_HASHRATE'], 
                          0.0, 
                          0.5, 
                          0.0, 
                          False, 
                          '')
            states.append(state)

        # Add a few randomly-timed FX jumps (up or down 10 and 15 percent) to
        # see how algos recalibrate
        fx_jumps = {}
        num_fx_jumps = 2 + params['num_blocks']/3000

        if 'price10x' in params['scenario'].params:
            factor_choices = [0.1, 0.25, 0.5, 2.0, 4.0, 10.0]
            for n in range(4):
                fx_jumps[random.randrange(params['num_blocks'])] = random.choice(factor_choices)
        elif not 'price1x' in params['scenario'].params:
            factor_choices = [0.85, 0.9, 1.1, 1.15]
            for n in range(10):
                fx_jumps[random.randrange(params['num_blocks'])] = random.choice(factor_choices)

        # Run the simulation
        if print_it:
            print_headers()
        for n in range(params['num_blocks']):
            fx_jump_factor = fx_jumps.get(n, 1.0)
            next_step(fx_jump_factor, params)
            if print_it:
                print_state()

        # Drop the prefix blocks to be left with the simulation blocks
        simul = states[N:]
    except:
        lock.release()
        raise
    lock.release()

    if returnstate:
        return simul

    block_times = [simul[n + 1].timestamp - simul[n].timestamp
                   for n in range(len(simul) - 1)]
    return block_times


# def main():
#     '''Outputs CSV data to stdout.   Final stats to stderr.'''

#     parser = argparse.ArgumentParser('Run a mining simulation')
#     # parser.add_argument('-a', '--algo', metavar='algo', type=str,
#     #                     choices = list(Algos.keys()),
#     #                     default = 'k-1', help='algorithm choice')
#     parser.add_argument('-a', '--algo', metavar='algo', type=str,
#                         choices = list(Algos.keys()),
#                         default = 'aserti3-416', help='algorithm choice')
#     parser.add_argument('-s', '--scenario', metavar='scenario', type=str,
#                         choices = list(Scenarios.keys()),
#                         default = 'default', help='scenario choice')
#     parser.add_argument('-r', '--seed', metavar='seed', type=int,
#                         default = None, help='random seed')
#     parser.add_argument('-n', '--count', metavar='count', type=int,
#                         default = 1, help='count of simuls to run')
#     args = parser.parse_args()

#     count = max(1, args.count)
#     algo = Algos.get(args.algo)
#     scenario = Scenarios.get(args.scenario)
#     seed = int(time.time()) if args.seed is None else args.seed

#     to_stderr = partial(print, file=sys.stderr)
#     to_stderr("Starting seed {} for {} simuls".format(seed, count))

#     means = []
#     std_devs = []
#     medians = []
#     maxs = []
#     for loop in range(count):
#         random.seed(seed)
#         seed += 1
#         block_times = run_one_simul(algo, scenario, count == 1)
#         means.append(statistics.mean(block_times))
#         std_devs.append(statistics.stdev(block_times))
#         medians.append(sorted(block_times)[len(block_times) // 2])
#         maxs.append(max(block_times))

#     def stats(text, values):
#         if count == 1:
#             to_stderr('{} {}s'.format(text, values[0]))
#         else:
#             to_stderr('{}(s) Range {:0.1f}-{:0.1f} Mean {:0.1f} '
#                       'Std Dev {:0.1f} Median {:0.1f}'
#                       .format(text, min(values), max(values),
#                               statistics.mean(values),
#                               statistics.stdev(values),
#                               sorted(values)[len(values) // 2]))

#     stats("Mean   block time", means)
#     stats("StdDev block time", std_devs)
#     stats("Median block time", medians)
#     stats("Max    block time", maxs)

# if __name__ == '__main__':
#     main()