#pragma once
#include "Scene.h"

class SceneTitle : public Scene
{
public:
	void Initialize() override;
	void Update() override;
	void Render() override;
	void Finalize() override;
};