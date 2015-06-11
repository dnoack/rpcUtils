/*
 * RPCMsg.cpp
 *
 *  Created on: 11.06.2015
 *      Author: dave2
 */

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


