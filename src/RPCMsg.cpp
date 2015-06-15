#include "RPCMsg.hpp"
#include "ComPoint.hpp"


bool RPCMsg::isOriginTcp()
{
	bool result = false;
	if(comPoint != NULL)
	{
		if(comPoint->getUniqueID() == TCP_SIDE)
			result = true;
	}
	return result;
}
