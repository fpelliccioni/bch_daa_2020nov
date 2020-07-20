import aserti3416cpp

block_index = aserti3416cpp.CBlockIndex_construct()
aserti3416cpp.CBlockIndex_set_nHeight(block_index, 123456)
print(aserti3416cpp.CBlockIndex_get_nHeight(block_index))
aserti3416cpp.CBlockIndex_destruct(block_index)