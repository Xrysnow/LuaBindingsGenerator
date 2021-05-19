#pragma once
#include "cocos2d.h"
#include "ui/UIWidget.h"
#include <unordered_set>

namespace l2d
{
	class Model : public cocos2d::ui::Widget
	{
	public:
		/**
		 * Create live2d model from directory and file name.
		 * Default size and anchor are correspond to the canvas.
		 */
		static Model* create(const std::string& dir, const std::string& fileName);

		// info

		uint32_t getMocVersion() const;
		cocos2d::Size getCanvasSize() const;
		cocos2d::Vec2 getCanvasOrigin() const;
		float getPixelsPerUnit() const;
		std::string getDirectory() const;
		std::vector<std::string> getMotionGroupNames() const;
		size_t getMotionCount(const std::string& groupName);
		std::vector<std::string> getExpressionNames() const;
		std::string getSoundFileName(const std::string& groupName, int32_t index);

		// operation

		bool startMotion(const char* groupName, int32_t index, int32_t priority);
		bool startRandomMotion(const char* groupName, int32_t priority);
		bool areaHitTest(const char* hitAreaName, float x, float y);
		bool setExpression(const char* expressionName);
		bool setRandomExpression();
		void setAutoDragging(bool b);
		void setDragging(float x, float y);
		void setTouchDragging(float x, float y);
		void resetDragging();
		void setAcceleration(float x, float y, float z);

		// property

		void setModelOpacity(uint8_t opacity);
		uint8_t getModelOpacity() const;
		void setModelColor(const cocos2d::Color4B& color);
		cocos2d::Color4B getModelColor() const;

		void setDebugRectEnable(bool b);
		cocos2d::DrawNode* getDebugRectRenderer() const;
		//void update(float delta) override;

	public:

		// parameter

		int32_t getParameterCount() const;
		std::vector<std::string> getParameterNames() const;
		float getParameterMax(const std::string& name) const;
		float getParameterMin(const std::string& name) const;
		float getParameterDefault(const std::string& name) const;
		float getParameter(const std::string& name) const;
		void setParameter(const std::string& name, float value, float weight = 1.0f);
		void addParameter(const std::string& name, float value, float weight = 1.0f);
		void multiplyParameter(const std::string& name, float value, float weight = 1.0f);

		// part

		int32_t getPartCount() const;
		std::vector<std::string> getPartNames() const;
		std::vector<int> getPartParents() const;
		float getPartOpacity(const std::string& name) const;
		void setPartOpacity(const std::string& name, float opacity);

		// drawable

		int32_t getDrawableCount() const;
		std::vector<std::string> getDrawableNames() const;
		float getDrawableOpacity(const std::string& name) const;
		int32_t getDrawableCulling(const std::string& name) const;
		int32_t getDrawableBlendMode(const std::string& name) const;

		// gravity, wind

		cocos2d::Vec2 getGravity() const;
		void setGravity(const cocos2d::Vec2& gravity);
		cocos2d::Vec2 getWind() const;
		void setWind(const cocos2d::Vec2& wind);

		// blinking, lip

		void setBlinkingInterval(float seconds);
		void setBlinkingSettings(float closing, float closed, float opening);
		bool getLipSync() const;
		void setLipSync(bool value);
		float getLipValue() const;
		void setLipValue(float value);

		void setSoundEventHandler(const std::function<void(const std::string&, int)>& f);

	};
}
