import numpy as np
import glob
import struct


chosen_order = "little"
byte_order = {
  "big":">",
  "little":"<"
  }
datatype= {
  "int":"l",
  "float":"f",
  "char":"c"
}

#filename = "./rle_encode/rle_input/xenon1_csr_ind_RLDin.txt"
filename = "./rle_encode/rle_input/g7jac160_csr_ind_RLDin"
location = "."+ filename.split('.')[1] + "_58B"
#location = "."+ filename.split('.')[1] + "_test"

threadsize =  5
print(location)
filecontent = open ( filename,"rb")
#filecontent = [0,5,1,4,6,6]
bytefile = open ( location ,"wb")
for item in filecontent:
  #print( "Write : ", int(item))
  #newitem = int(int(item) & int( '0x00000007',16))
  #newitem = int(int(item) & int( '0x0000003F',16))
  newitem = int(int(item) %57 )
  bytefile.write( struct.pack( byte_order[chosen_order] + datatype["int"], int(newitem)))
  newitem = int(int(item) % 10)
  if newitem ==0:
    newitem = 1
  bytefile.write( struct.pack( byte_order[chosen_order] + datatype["int"], int(newitem)))
bytefile.close()
filecontent.close()
