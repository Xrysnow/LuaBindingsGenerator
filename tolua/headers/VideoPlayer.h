#pragma once
#include "cocos2d.h"
#include <functional>

namespace video
{
	class Player : public cocos2d::Sprite
	{
	public:
		enum PlayMode
		{
			STEP,	   // step a const time per frame
			REALTIME,  // realtime time -> video time, not recommand
			FRAME,	   // game frame -> video frame
			MANUAL,	   // invoke update manually
		};
		static Player* create(const std::string& path);
	public:
		void vplay();
		void vstop();
		void vpause();
		void vresume();
		// can only seek key frame, not recommand to set a non-0 value
		void seek(uint32_t frame);
		// step dt in decoder and get data, will step 1 frame if dt<0
		void update(float dt) override;

		// see PlayMode
		void setPlayMode(PlayMode mode);
		// used for PlayMode::STEP mode
		void setStep(double v);
		// auto invoke removeFromParentAndCleanup when finished
		void setAutoRemove(bool b);
		// save current frame to file
		void saveCurrentFrame(const std::string& path);

	protected:
		Player();
		virtual ~Player();
	};
}
