#ifndef DMZ_PLUGIN_FOLLOW_ME_DOT_H
#define DMZ_PLUGIN_FOLLOW_ME_DOT_H

#include <dmzObjectObserverUtil.h>
#include <dmzRuntimeLog.h>
#include <dmzRuntimePlugin.h>
#include <dmzTypesMask.h>
#include <dmzTypesVector.h>

namespace dmz {

   class RenderModuleIsect;

   class PluginFollowMe :
         public Plugin,
         public ObjectObserverUtil {

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

      protected:
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

         Float64 _minDistance;

         Vector _targetPosition;

      private:
         PluginFollowMe ();
         PluginFollowMe (const PluginFollowMe &);
         PluginFollowMe &operator= (const PluginFollowMe &);
   };
};

#endif // DMZ_PLUGIN_FOLLOW_ME_DOT_H
