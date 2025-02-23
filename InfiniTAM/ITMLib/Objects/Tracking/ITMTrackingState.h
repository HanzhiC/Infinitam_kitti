// Copyright 2014-2017 Oxford University Innovation Limited and the authors of InfiniTAM

#pragma once

#include "../../../ORUtils/SE3Pose.h"
#include "../Misc/ITMPointCloud.h"
#include <iostream>

namespace ITMLib
{
	/** \brief
	    Stores some internal variables about the current tracking
	    state, most importantly the camera pose
	*/
	class ITMTrackingState
	{
	public:
		/** @brief
		    Excerpt of the scene used by the tracker to align
		    a new frame.

		    This is usually the main result generated by the
		    raycasting operation in a ITMLib::Engine::ITMSceneReconstructionEngine.
		*/
		ITMPointCloud *pointCloud;

		/// The pose used to generate the point cloud.
		ORUtils::SE3Pose *pose_pointCloud;

		/// Frames processed from start of tracking
		/// Used as weight in the extended tracker 
		int framesProcessed;

		int age_pointCloud;

		/// Current pose of the depth camera.
		ORUtils::SE3Pose *pose_d;

		/// Tracking quality: 1.0: success, 0.0: failure
		enum TrackingResult
		{
			TRACKING_GOOD = 2,
			TRACKING_POOR = 1,
			TRACKING_FAILED = 0
		} trackerResult;

		/// Score associated to the tracking result.
		float trackerScore;

		bool HasValidPointCloud(void) const
		{
			return age_pointCloud != -1;
		}

		bool TrackerFarFromPointCloud(void) const
		{
			// if no point cloud exists, yet
			if (age_pointCloud < 0) return true;
			// if the point cloud is older than n frames
			if (age_pointCloud > 5) return true;

			Vector3f cameraCenter_pc = -1.0f * (pose_pointCloud->GetR().t() * pose_pointCloud->GetT());
			Vector3f cameraCenter_live = -1.0f * (pose_d->GetR().t() * pose_d->GetT());

			Vector3f diff3 = cameraCenter_pc - cameraCenter_live;

			float diff = diff3.x * diff3.x + diff3.y * diff3.y + diff3.z * diff3.z;
//            std::cout << diff <<std::endl;
			// if the camera center has moved by more than a threshold
			if (diff > 0.0005f) return true;

			return false;
		}

		ITMTrackingState(Vector2i imgSize, MemoryDeviceType memoryType)
		: pointCloud(new ITMPointCloud(imgSize, memoryType)),
			pose_pointCloud(new ORUtils::SE3Pose),
			pose_d(new ORUtils::SE3Pose)
		{
			Reset();
		}

		~ITMTrackingState(void)
		{
			delete pointCloud;
			delete pose_d;
			delete pose_pointCloud;
		}

		void Reset()
		{
			this->age_pointCloud = -1;
			this->pose_d->SetFrom(0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f);
			this->pose_pointCloud->SetFrom(0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f);
			this->trackerResult = TRACKING_GOOD;
			this->trackerScore = 0.0f;
		}

		// Suppress the default copy constructor and assignment operator
		ITMTrackingState(const ITMTrackingState&);
		ITMTrackingState& operator=(const ITMTrackingState&);
	};
}
