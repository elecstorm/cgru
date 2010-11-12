#include "monitoraf.h"

#include "../libafanasy/environment.h"
#include "../libafanasy/msgclasses/mcgeneral.h"

#include "monitorcontainer.h"

#define AFOUTPUT
#undef AFOUTPUT
#include "../include/macrooutput.h"

MonitorAf::MonitorAf( af::Msg * msg, const af::Address * addr):
   af::Monitor( msg, addr)
{
}

MonitorAf::~MonitorAf()
{
}

void MonitorAf::refresh( time_t currentTime, AfContainer * pointer, MonitorContainer * monitoring)
{
   if( getTimeUpdate() < (currentTime - af::Environment::getMonitorZombieTime()))
   {
      if( monitoring) monitoring->addEvent( af::Msg::TMonitorMonitorsDel, id);
      setZombie();
   }
}

void MonitorAf::setZombie()
{
   af::printTime(); printf(" : Deleting Monitor: "); stdOut( false);

   Node::setZombie();
}

bool MonitorAf::action( const af::MCGeneral & mcgeneral, int type, AfContainer * pointer, MonitorContainer * monitoring)
{
   switch( type)
   {
   case af::Msg::TMonitorExit:
   {
      MsgAf* msg = new MsgAf( af::Msg::TClientExitRequest);
      msg->setAddress( this);
      msg->dispatch();
      return true;
   }
   default:
   {
      return false;
   }
   }
   return true;
}

bool MonitorAf::setInterest( int type, const af::MCGeneral & ids)
{
   time_activity = time( NULL);
//printf("MonitorAf::setInterest: [%s]:\n", af::Msg::TNAMES[type]);
   switch(type)
   {
      case af::Msg::TMonitorSubscribe:
      {
         setEvents( ids, true);
         break;
      }
      case af::Msg::TMonitorUnsubscribe:
      {
         setEvents( ids, false);
         break;
      }
      case af::Msg::TMonitorUsersJobs:
      {
         setJobsUsersIds( ids);
         break;
      }
      case af::Msg::TMonitorJobsIdsAdd:
      {
         addJobIds( ids);
         break;
      }
      case af::Msg::TMonitorJobsIdsSet:
      {
         setJobIds( ids);
         break;
      }
      case af::Msg::TMonitorJobsIdsDel:
      {
         delJobIds( ids);
         break;
      }
      default:
      {
         AFERRAR("MonitorAf::action: Unknown action: %s\n", af::Msg::TNAMES[type]);
         return false;
      }
   }
   return true;
}

void MonitorAf::setJobsUsersIds( const af::MCGeneral & ids)
{
   int count = ids.getCount();
   jobsUsersIds.clear();
   for( int i = 0; i < count; i++) jobsUsersIds.push_back( ids.getId( i));
}

void MonitorAf::setEvents( const af::MCGeneral & ids, bool value)
{
   int count = ids.getCount();
   for( int i = 0; i < count; i++)
   {
      int eventNum = ids.getId(i) - EventsShift;
//printf("MonitorAf::setEvents: [%s] - %s\n", af::Msg::TNAMES[ids.getId(i)], value ? "ADD" : "DEL");
      if((eventNum >= 0) && (eventNum < EventsCount))
      {
         events[eventNum] = value;
      }
      else
      {
         AFERRAR("MonitorAf::addEvent: Invalid event: [%s]\n", af::Msg::TNAMES[ids.getId(i)]);
      }
   }
}

bool MonitorAf::hasJobUid( int uid) const
{
   for( std::list<int32_t>::const_iterator it = jobsUsersIds.begin(); it != jobsUsersIds.end(); it++)
      if( *it == uid) return true;
   return false;
}

bool MonitorAf::hasJobEvent( int type, int uid) const
{
   int count = jobsUsersIds.size();
   if( count < 1) return false;
   if( hasEvent( type) == false) return false;

   std::list<int32_t>::const_iterator it = jobsUsersIds.begin();
   if(( count == 1 ) && ( *it == 0 )) return true;

   return hasJobUid( uid);
}

bool MonitorAf::hasJobId( int id) const
{
   for( std::list<int32_t>::const_iterator it = jobsIds.begin(); it != jobsIds.end(); it++)
   {
      if( *it == id) return true;
   }
   return false;
}

void MonitorAf::addJobIds( const af::MCGeneral & ids)
{
   int count = ids.getCount();
   for( int i = 0; i < count; i++)
   {
      int id = ids.getId(i);
      if( hasJobId( id) == false)
      {
         jobsIds.push_back( id);
      }
   }
}

void MonitorAf::setJobIds( const af::MCGeneral & ids)
{
   int count = ids.getCount();
   jobsIds.clear();
   for( int i = 0; i < count; i++) jobsIds.push_back( ids.getId(i));
}

void MonitorAf::delJobIds( const af::MCGeneral & ids)
{
   int count = ids.getCount();
   for( int i = 0; i < count; i++) jobsIds.remove( ids.getId(i));
}
