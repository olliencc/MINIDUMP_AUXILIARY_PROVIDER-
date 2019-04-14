#
# April 2019
# Ollie Whitehouse
# ollie.whitehouse [@] nccgroup.com
# https://github.com/olliencc/MINIDUMP_AUXILIARY_PROVIDER-
#

# https://github.com/dotnet/coreclr/blob/cbd672eb2735d583ee6fa46aaf599241fdf6face/src/debug/daccess/dacdbiimpl.h
# https://github.com/dotnet/coreclr/blob/2573e4e5c12c4fd165db53cc31b0fc14c118d47b/src/dlls/mscordac/mscordac_unixexports.src
# https://github.com/dotnet/coreclr/blob/030a3ea9b8dbeae89c90d34441d4d9a1cf4a7de6/src/dlls/mscordac/mscordac.src
# https://github.com/search?q=org%3Adotnet+DacDbiInterfaceInstance&type=Code
# https://github.com/dotnet/docs/blob/4ef9b84b8f41a01ce312574d9c1a6147275adbff/docs/framework/unmanaged-api/debugging/clrdatacreateinstance-function.md

import os
import pefile

rootdirs = [
           #"C:/Data/NCC/!Research/DUMPHELPER/test/t/"
           "C:/Windows/",
           "C:/Program Files/",
           "C:/Program Files (X86)/"
           ]

for dir in rootdirs:
    for subdir, dirs, files in os.walk(dir):
        for file in files:
            try:
                myPE = pefile.PE(os.path.join(subdir, file))

                # in part based on https://github.com/matonis/ripPE/blob/master/ripPE.py
                # but some fixes to get the names of the entries in the directories

                
                if hasattr(myPE, 'DIRECTORY_ENTRY_RESOURCE'):
                    for resource_type in myPE.DIRECTORY_ENTRY_RESOURCE.entries:
                        name = ""
                        if resource_type.name is not None:
                            name = resource_type.name

                            # NOTE: the actual name is called 'MINIDUMP_AUXILIARY_PROVIDER'
                            # https://github.com/dotnet/coreclr/tree/master/src/dlls/mscoree/coreclr
                            # https://github.com/dotnet/coreclr/issues/10334
                            # https://github.com/dotnet/coreclr/blob/master/src/dlls/mscoree/coreclr/CMakeLists.txt
                            # https://github.com/dotnet/coreclr/pull/10336

                        else:
                            name = "1 - %s" % pefile.RESOURCE_TYPE.get(resource_type.struct.Id)

                        if name == None:
                            name = "2 - %d" % resource_type.struct.Id

                        # print (os.path.join(subdir, file) + " " + name)

                        if hasattr(resource_type, 'directory'):
                            for resource_id in resource_type.directory.entries:
                                name2 = ""
                                
                                if resource_id.name is not None:
                                    name2 = resource_id.name
                                else:
                                    name2 = "%s" % pefile.RESOURCE_TYPE.get(resource_id.struct.Id)

                                # print (os.path.join(subdir, file) + " 3 - " + str(name2))

                                if(str(name2) == "MINIDUMP_AUXILIARY_PROVIDER"):
                                    print (os.path.join(subdir, file) + " - " + str(name2))

                                #if hasattr(resource_id, 'directory'):
                                #    for resource_id2 in resource_type.directory.entries:
                                #        name3 = ""
                                #
                                #        if resource_id2.name is not None:
                                #            name3 = resource_id2.name
                                #        else:
                                #            name3 = "%s" % pefile.RESOURCE_TYPE.get(resource_id2.struct.Id)

                                #        print (os.path.join(subdir, file) + " 4 - " + str(name3))

            except:
                continue
