// This file was written by Lucas Saragosa. The code derives from Telltale Games' Engine.
// I do not intend to take credit for it, however; Im the author of this interpretation of 
// the engine and require that if you use this code or library, you give credit to me and
// the amazing Telltale Games.

#ifndef _PRELOADPKG
#define _PRELOADPKG

#include "../Meta.hpp"
#include "DCArray.h"
#include "Dialog.h"
#include "Set.h"

namespace PreloadPackage {

	struct ResourceKey {
		Symbol mResourceName;
		u64 mMetaClassDescriptionCrc;
		//NEWER GAMES
		BitSet<enum RenderQualityType, 5, 0> mRenderQualities;
		//EVEN NEWER GAMES
		bool mVisible;
		String mPrefix;
		//----

		inline bool operator<(const ResourceKey& rhs) const {
			return mResourceName < rhs.mResourceName;
		}

	};

	struct StartNodeOffset {
		DlgObjID mStartNodeChain;
		float fStartTimeSeconds;
		float mfMinDurationToPreload;
	};

	struct ResourceSeenTimes {
		float mfEarliest;
		float mfLatest;
		Set<Symbol> mAdditionalScenes;
	};

	//.PRELOADPACKAGERTD FILES
	struct RuntimeDataDialog {

		struct DialogResourceInfo {
			ResourceKey mResourceKey;
			ResourceSeenTimes mResourceSeenTimes;
		};

		struct DlgObjIdAndResourceVector {
			DlgObjID mID;
			DCArray<DialogResourceInfo> mVector;
		};

		struct DlgObjIdAndStartNodeOffset {
			DlgObjID mID;
			StartNodeOffset mOffset;
		};

		DCArray<DlgObjIdAndResourceVector> mDialogResourceVectors;
		DCArray<DlgObjIdAndStartNodeOffset> mStartNodeOffsets;

	};

	//.PRELOADPACKAGERTS FILES
	struct RuntimeDataScene {
		DCArray<ResourceKey> mResources;
		//String mObjectName;
	};

}

#endif