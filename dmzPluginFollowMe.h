#ifndef DMZ_PLUGIN_FOLLOW_ME_DOT_H
#define DMZ_PLUGIN_FOLLOW_ME_DOT_H

#include <dmzEventObserverUtil.h>
#include <dmzObjectObserverUtil.h>
#include <dmzRuntimeEventType.h>
#include <dmzRuntimeLog.h>
#include <dmzRuntimePlugin.h>
#include <dmzRuntimeTimeSlice.h>
#include <dmzTypesMask.h>
#include <dmzTypesVector.h>

namespace dmz {

   class RenderModuleIsect;

   class PluginFollowMe :
         public Plugin,
         public TimeSlice,
         public ObjectObserverUtil,
         public EventObserverUtil {

      public:
         PluginFollowMe (const PluginInfo &Info, Config &local);
         ~PluginFollowMe ();

         // Plugin Interface
         virtual void update_plugin_state (
            const PluginStateEnum State,
            const UInt32 Level);

         virtual void discover_plugin (
            const PluginDiscoverEnum Mode,
            const Plugin *PluginPtr);

         // TimeSlice Interface
         virtual void update_time_slice (const Float64 TimeDelta);

         // Object Observer Interface
         virtual void update_object_flag (
            const UUID &Identity,
            const Handle ObjectHandle,
            const Handle AttributeHandle,
            const Boolean Value,
            const Boolean *PreviousValue);

         virtual void update_object_position (
            const UUID &Identity,
            const Handle ObjectHandle,
            const Handle AttributeHandle,
            const Vector &Value,
            const Vector *PreviousValue);

         // Event Observer Interface
         virtual void close_event (
            const Handle EventHandle,
            const EventType &Type,
            const EventLocalityEnum Locality);

      protected:
         void _move (
            const Float64 TimeDelta,
            const Vector Pos,
            const Matrix Ori,
            Vector &pos);

         void _turn_and_clamp (
            const Vector Pos,
            Vector &newPos,
            Matrix &newOri);

         void _init (Config &local);

         Log _log;

         RenderModuleIsect *_isect;

         Handle _hil;
         Handle _me;

         Handle _defaultAttrHandle;
         Handle _hilAttrHandle;
         Handle _targetAttrHandle;

         EventType _detonationType;

         Vector _targetPosition;

         Float64 _minDistance;
         Float64 _speed;

         Mask _deadState;
         Mask _smokeAndFireState;

      private:
         PluginFollowMe ();
         PluginFollowMe (const PluginFollowMe &);
         PluginFollowMe &operator= (const PluginFollowMe &);
   };
};

#endif // DMZ_PLUGIN_FOLLOW_ME_DOT_H
