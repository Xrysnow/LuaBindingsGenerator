﻿#pragma once
#include "cocos2d.h"

namespace lstg
{
	class WindowHelper : public cocos2d::Ref
	{
	public:
		enum Operation
		{
			SHOW,
			HIDE,
			FOCUS,	
			ICONIFY,
			RESTORE,
			MAXIMIZE
		};
		enum CursorType
		{
			ARROW,
			IBEAM,
			CROSSHAIR,
			HAND,
			HRESIZE,
			VRESIZE,
			CUSTOM
		};

		virtual ~WindowHelper() = default;
		static WindowHelper* getInstance();

		virtual void setVsync(bool b) {}
		virtual bool isVsync() { return true; }

		virtual void operate(Operation op) {}

		virtual void setGamma(float v){}
		virtual float getGamma() { return 0.f; }

		virtual void setClipboardString(const std::string& s) {}
		virtual std::string getClipboardString() { return ""; }

		virtual void hint(int hint, int value){}
		virtual void resetHint(){}
	};

	class WindowHelperDesktop : public WindowHelper
	{
	public:
		struct VideoMode
		{
			int width = 0;
			int height = 0;
			bool windowed = true;
			bool vsync = true;
			VideoMode(int w, int h, bool windowed_, bool vsync_)
			:width(w), height(h), windowed(windowed_), vsync(vsync_)
			{
			}
		};

		static WindowHelperDesktop* getInstance();

		virtual void setTitle(const std::string& s);
		virtual std::string getTitle();

		virtual void setPosition(const cocos2d::Vec2& p);
		virtual cocos2d::Vec2 getPosition();

		virtual void setVisible(bool b);
		virtual bool isVisible();

		virtual void setCursorVisible(bool b);
		virtual bool isCursorVisible();

		virtual void setSize(const cocos2d::Size& size);
		virtual cocos2d::Size getSize();

		virtual void setFullscreen();
		virtual bool isFullscreen();

		void setVsync(bool b) override;
		bool isVsync() override;

		void setVideoMode(VideoMode v);
		VideoMode getVideoMode();

		virtual void moveToCenter();

		void operate(Operation op) override;

		void setGamma(float v) override;
		float getGamma() override;

		void setClipboardString(const std::string& s) override;
		std::string getClipboardString() override;

		void hint(int hint, int value) override;
		void resetHint() override;

		void setCustomCursor(const std::string& filename, const cocos2d::Vec2& hotspot);
		void setDefaultCursor();
		void setStandardCursor(CursorType type);
	};

}
