#include "talk.h"

#include <stdio.h>

#include "environment.h"
#include "address.h"
#include "msg.h"

using namespace af;

Talk::Talk():
   Client( Client::GetEnvironment, 0)
{
   name = QString("%1@%2:%3").arg(af::Environment::getUserName(), af::Environment::getHostName()).arg( address->getPortString());
}

Talk::Talk( Msg * msg, const Address * addr):
   Client( Client::DoNotGetAnyValues, 0)
{
   read( msg);
   if( addr != NULL) address->setIP( addr);
}

Talk::~Talk()
{
}

void Talk::readwrite( Msg * msg)
{
   rw_int32_t ( id,            msg);
   rw_uint32_t( time_launch,   msg);
   rw_uint32_t( time_update,   msg);
   rw_uint32_t( time_register, msg);
   rw_QString ( name,          msg);
   rw_QString ( username,      msg);
   rw_int32_t(  revision,      msg);
   rw_QString(  version,       msg);
   if( msg->isWriting() ) address->write( msg);
   else address = new Address( msg);
}

void Talk::stdOut( bool full) const
{
   printf("%s@%s  :: ", name.toUtf8().data(), username.toUtf8().data());
   address->stdOut();
   printf("\n");
   printf("   Version: %s rev%d\n", version.toUtf8().data(), revision);
}
