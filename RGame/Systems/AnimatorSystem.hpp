#pragma once

#include "../Components/Components.hpp"


class AnimatorSystem
{
public:
	void Update(entt::registry& registry)
	{
		auto view = registry.view<Animator>();

		for (auto [entity, animator] : view.each())
		{
			if (animator.ToPlay != "")
			{
				int id = -1;
				for (size_t i = 0; i < animator.anims.size(); i++)
				{
					if (animator.anims[i].name == animator.ToPlay)
					{
						id = i;
						break;
					}
				}

				if (id == -1)
				{
					spdlog::warn("Could not play animation {}, animation not found inside an object", animator.ToPlay);
					animator.ToPlay = "";
					return;
				}

				animator.currentAnimationId = id;
				animator.currentAnimationName = animator.ToPlay;
				animator.ToPlay = "";
				animator.currentFrame = 0;
				animator.currentTime = animator.anims[animator.currentAnimationId].frames[0].duration;
				

				if (auto* sprite = registry.try_get<Sprite>(entity))
				{
					sprite->textureRect = animator.anims[animator.currentAnimationId].frames[animator.currentFrame].frame;
					sprite->useTextureRect = true;
				}
				else
				{
					spdlog::warn("Could not find Sprite on an Object with an Animator Component, Texture Rect not changed");
				}


				spdlog::info("Successfully started playing {} ", animator.currentAnimationName);
			}
			else
			{
				if (animator.currentAnimationId != -1)
				{
					animator.currentTime -= dt;
					if (animator.currentTime <= 0)
					{
						animator.currentFrame++;

						if (animator.currentFrame >= animator.anims[animator.currentAnimationId].frames.size())
						{
							if (animator.looping)
							{
								animator.currentFrame = 0;
								animator.currentTime = animator.anims[animator.currentAnimationId].frames[0].duration;

							}
							else
							{
								animator.currentAnimationId = -1;
								animator.currentAnimationName = "";
								animator.currentFrame = 0;
								animator.currentTime = 0;
								continue;
							}
						}
						else
						{
							animator.currentTime = animator.anims[animator.currentAnimationId].frames[animator.currentFrame].duration;
						}
						
						if (auto* sprite = registry.try_get<Sprite>(entity)) 
						{
							sprite->textureRect = animator.anims[animator.currentAnimationId].frames[animator.currentFrame].frame;
							sprite->useTextureRect = true;
						}
						else
						{
							spdlog::warn("Could not find Sprite on an Object with an Animator Component, Texture Rect not changed");
						}

					}
				}
			}
		}
	}
};