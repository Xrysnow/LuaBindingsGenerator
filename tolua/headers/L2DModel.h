#pragma once
#include "cocos2d.h"
#include "ui/UIWidget.h"
#include <unordered_set>

namespace l2d
{
	class Model : public cocos2d::ui::Widget
	{
	public:
		static Model* create(const std::string& dir, const std::string& fileName);

		bool startMotion(const char* group, int32_t no, int32_t priority);
		bool startRandomMotion(const char* group, int32_t priority);

		bool areaHitTest(const char* hitAreaName, float x, float y);
		bool setExpression(const char* expressionID);
		bool setRandomExpression();

		std::vector<std::string> getMotionNames() const; // group_no
		std::vector<std::string> getExpressionNames() const;
		std::vector<std::string> getMotionGroupNames() const;

		void onEnter() override;
		void onExit() override;

		void update(float delta) override;

		void draw(cocos2d::Renderer* renderer, const cocos2d::Mat4& transform, uint32_t flags) override;

		void setModelOpacity(uint8_t opacity);
		uint8_t getModelOpacity() const;

		void setModelColor(const cocos2d::Color4B& color);
		cocos2d::Color4B getModelColor() const;

		void setDragging(float x, float y);
		void setTouchDragging(float x, float y);
		void resetDragging();
		void setAcceleration(float x, float y, float z);

		cocos2d::Size getCanvasSize() const;

		void setDebugRectEnable(bool b);
		cocos2d::DrawNode* getDebugRectRenderer() const;

	public:

		int32_t getParameterCount() const;
		std::vector<std::string> getParameterNames() const;
		float getParameterMaximumValue(const std::string& name) const;
		float getParameterMinimumValue(const std::string& name) const;
		float getParameterDefaultValue(const std::string& name) const;
		float getParameterValue(const std::string& name) const;
		void  setParameterValue(const std::string& name, float value, float weight = 1.0f);
		void  addParameterValue(const std::string& name, float value, float weight = 1.0f);
		void  multiplyParameterValue(const std::string& name, float value, float weight = 1.0f);

		int32_t getPartCount() const;
		std::vector<std::string> getPartNames() const;
		float getPartOpacity(const std::string& name) const;
		void setPartOpacity(const std::string& name, float opacity);

		int32_t getDrawableCount() const;
		std::vector<std::string> getDrawableNames() const;
		float getDrawableOpacity(const std::string& name) const;
		int32_t getDrawableCulling(const std::string& name) const;
		int32_t getDrawableBlendMode(const std::string& name) const;

		cocos2d::Vec2 getGravity() const;
		void setGravity(const cocos2d::Vec2& gravity);
		cocos2d::Vec2 getWind() const;
		void setWind(const cocos2d::Vec2& wind);

		void setBlinkingInterval(float blinkigInterval);// in seconds
		void setBlinkingSettings(float closing, float closed, float opening);

		float getLipValue() const;
		void setLipValue(float value);
	};
}
