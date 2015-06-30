INSTALL_PATH='RSD-Framework'
CURRENT_PATH="$PWD"

mkdir -p ${INSTALL_PATH}
cd ${INSTALL_PATH}

git clone https://github.com/miloyip/rapidjson
git clone https://github.com/dnoack/rpcUtils
git clone https://github.com/dnoack/Remote-Server-Daemon
git clone https://github.com/dnoack/Aardvark-Plugin
git clone https://github.com/dnoack/I2C-Plugin


LD_LIBRARY_PATH="${CURRENT_PATH}/${INSTALL_PATH}/rpcUtils/Release:${CURRENT_PATH}/${INSTALL_PATH}/Aardvark-Plugin/include/aardvark"
export LD_LIBRARY_PATH

#CLFAGS for building rpcUtils
CFLAGS="-I${CURRENT_PATH}/${INSTALL_PATH}//rpcUtils/include -I${CURRENT_PATH}/${INSTALL_PATH}/rapidjson/include/rapidjson"

#CFLAGS for building RSD
CFLAGS="$CFLAGS -I${CURRENT_PATH}/${INSTALL_PATH}/Remote-Server-Deamon/include"

#CFLAGS for build Aardvark Plugin
CFLAGS="$CFLAGS -I${CURRENT_PATH}/${INSTALL_PATH}/Aardvark-Plugin/include"

#CFLAGS for building I2C-Plugin
CFLAGS="$CFLAGS -I${CURRENT_PATH}/${INSTALL_PATH}/I2C-Plugin/include"

export CFLAGS
export CXXFLAGS=${CFLAGS}


#Linking for aardvark and rpcUtils shared library
LDFLAGS="-L${CURRENT_PATH}/${INSTALL_PATH}/Aardvark-Plugin/include/aardvark -L${CURRENT_PATH}/${INSTALL_PATH}/rpcUtils/Release"

export LDFLAGS

cd rpcUtils/Release
make clean
make

cd ${CURRENT_PATH}/${INSTALL_PATH}/Remote-Server-Daemon/Release
make clean
make

cd ${CURRENT_PATH}/${INSTALL_PATH}/Aardvark-Plugin/Release
make clean
make

cd ${CURRENT_PATH}/${INSTALL_PATH}/I2C-Plugin
make clean
make
