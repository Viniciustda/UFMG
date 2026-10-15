//
// Created by Lucas N. Ferreira on 03/08/23.
//

#include "Mario.h"
#include "Block.h"
#include "Mushroom.h"
#include "Goomba.h"
#include "Coin.h"
#include "../Game.h"
#include "../Components/Drawing/AnimatorComponent.h"
#include "../Components/Physics/RigidBodyComponent.h"
#include "../Components/Physics/AABBColliderComponent.h"
#include "../Components/ParticleSystemComponent.h"

Mario::Mario(Game* game, const float forwardSpeed, const float jumpSpeed)
        : Actor(game)
        , mIsRunning(false)
        , mIsDead(false)
        , mIsSuper(false)
        , mForwardSpeed(forwardSpeed)
        , mJumpSpeed(jumpSpeed)
        , mInvincibilityTimer(0.0f)
        , mStompTimer(0.0f)
        , mTurnTimer(0.0f)
        , mPrevScaleX(1.0f)
{
    // Set up animator component
    auto* ac = new AnimatorComponent(this, "../Assets/Sprites/Mario/Mario.png", "../Assets/Sprites/Mario/Mario.json", Game::TILE_SIZE, Game::TILE_SIZE);
    ac->AddAnimation("dead", {0});
    ac->AddAnimation("idle", {1});
    ac->AddAnimation("jump", {2});
    ac->AddAnimation("run", {3, 4, 5});
    ac->AddAnimation("stomp", {6, 7});  
    ac->AddAnimation("turn", {8});

    ac->SetAnimation("idle");
    ac->SetAnimFPS(10.0f);

    // Set up rigid body component
    auto* rbc = new RigidBodyComponent(this, 1.0f, 5.0f, true);

    // Set up collider component
    new AABBColliderComponent(this, 0, 0, Game::TILE_SIZE, Game::TILE_SIZE, ColliderLayer::Player);
}

void Mario::OnProcessInput(const uint8_t* state)
{
    if (mIsDead) {
        return;
    }

    auto* rbc = GetComponent<RigidBodyComponent>();
    if (!rbc) {
        return;
    }

    // Get current velocity
    Vector2 vel = rbc->GetVelocity();
    mIsRunning = false;

    // remember previous facing to detect quick turn
    float prevScale = mScale.x;

    // Apply forces based on key inputs
    if (state[SDL_SCANCODE_D]) {
        rbc->ApplyForce(Vector2(mForwardSpeed, 0.0f));
        mScale.x = 1.0f;
        mIsRunning = true;
    }
    else if (state[SDL_SCANCODE_A]) { 
        rbc->ApplyForce(Vector2(-mForwardSpeed, 0.0f));
        mScale.x = -1.0f;
        mIsRunning = true;
    }

    // If changed facing while we had horizontal motion, show turn animation briefly
    // (use persisted prevScale to detect changes across frames)
    if (prevScale != mScale.x && fabs(rbc->GetVelocity().x) > 0.1f) {
        auto* ac = GetComponent<AnimatorComponent>();
        if (ac) {
            ac->SetAnimation("turn");
            mTurnTimer = 0.20f; 
        }
    }

    // Jumping
    if (state[SDL_SCANCODE_W] && mIsOnGround) {
        Vector2 vel = rbc->GetVelocity();
        vel.y = mJumpSpeed;
        rbc->SetVelocity(vel);
        SetOffGround();
    }
}

void Mario::OnUpdate(float deltaTime)
{
    // Update invincibility timer
    if (mInvincibilityTimer > 0.0f) {
        mInvincibilityTimer -= deltaTime;
    }

    // Update stomp/turn timers
    if (mStompTimer > 0.0f) {
        mStompTimer -= deltaTime;
    }

    if (mTurnTimer > 0.0f) {
        mTurnTimer -= deltaTime;
    }

    // Prevent Mario from going left off the screen
    float leftEdge = mGame->GetCameraPos().x;

    float marioLeft = mPosition.x - (Game::TILE_SIZE / 2.0f);

    if (marioLeft < leftEdge) {
        mPosition.x = leftEdge + (Game::TILE_SIZE / 2.0f);
    }

    // Check if Mario is falling
    auto* rbc = GetComponent<RigidBodyComponent>();
    if (rbc && rbc->GetVelocity().y != 0.0f) {
        mIsOnGround = false;
    }

    // If Mario falls below the window, he dies
    // But if he is already dead, he is destroyed
    if (mPosition.y > Game::WINDOW_HEIGHT + Game::TILE_SIZE){
        if(mIsDead) {
            mState = ActorState::Destroy;
        }
        else {
            Kill();
        }
    }

    // Update animations based on state
    ManageAnimations();
}

void Mario::ManageAnimations()
{
    auto* ac = GetComponent<AnimatorComponent>();
    if (!ac) {
        return;
    }

    // Priority animations: stomp > turn > normal states
    if (mStompTimer > 0.0f) {
        ac->SetAnimation("stomp");
        return;
    }
    if (mTurnTimer > 0.0f) {
        ac->SetAnimation("turn");
        return;
    }

    // Update animation based on state
    if (!mIsDead) {
        if (mIsOnGround) {
            if (mIsRunning) {
                ac->SetAnimation("run");
            }
            else {
                ac->SetAnimation("idle");
            }
        }
        else {
            ac->SetAnimation("jump");
        }
    }
}

void Mario::Kill()
{
    // If already dead or invincible, do nothing
    if (mIsDead || mInvincibilityTimer > 0.0f) {
        return;
    }

    // If super, power down instead of dying
    if (mIsSuper) {
        PowerDown();
        return; 
    }

    
    mIsDead = true;

    // Set dead animation and disable components
    auto* ac = GetComponent<AnimatorComponent>();
    if (ac) {
        ac->SetAnimation("dead");
    }

    auto* cc = GetComponent<AABBColliderComponent>();
    if (cc) {
        cc->SetEnabled(false);
    }

    auto* rbc = GetComponent<RigidBodyComponent>();
    if (rbc) {
        rbc->SetVelocity(Vector2(0.0f, -500.0f));
    }

    GetGame()->SetMarioIsDead(true);
    
}

void Mario::OnHorizontalCollision(const float minOverlap, AABBColliderComponent* other)
{
    // If colliding with an enemy, check if we should die
    if (other->GetLayer() == ColliderLayer::Enemy) {
        auto* rbc = GetComponent<RigidBodyComponent>();
        auto* marioCollider = GetComponent<AABBColliderComponent>();
        auto* enemyCollider = other;

        if (rbc && marioCollider && enemyCollider) {
            // Make sure Mario is falling
            bool isFalling = rbc->GetVelocity().y > 0.0f;

            // Check if Mario's feet are above the enemy's center
            float marioFeetY = marioCollider->GetMax().y;
            float enemyCenterY = enemyCollider->GetMin().y + (Game::TILE_SIZE / 2.0f);
            bool isAboveEnemy = marioFeetY <= enemyCenterY;

            // If both conditions are true, it's a stomp.
            if (isFalling && isAboveEnemy) {
                Goomba* goomba = dynamic_cast<Goomba*>(other->GetOwner());
                if (goomba) {
                    goomba->setStomped(true);
                }
                other->GetOwner()->Kill();
                
                // Bounce Mario up
                Vector2 vel = rbc->GetVelocity();
                vel.y = mJumpSpeed * 0.6f; 
                rbc->SetVelocity(vel);
                SetOffGround();

                // show stomp animation briefly
                auto* ac = GetComponent<AnimatorComponent>();
                if (ac) {
                    ac->SetAnimation("stomp");
                    mStompTimer = 0.40f;
                }
            }
            else {
                // Otherwise, it's a normal side collision and Mario dies.
                Kill();
            }
        }
        else {
            // Fallback if any component is not found
            Kill();
        }
    }

    // If colliding with a collectable, check if we should power up
    if (other->GetLayer() == ColliderLayer::Collectable) {
        if (dynamic_cast<Mushroom*>(other->GetOwner())) {
            PowerUp();
            other->GetOwner()->Kill();
        }
        else if (dynamic_cast<Coin*>(other->GetOwner())) {
            GetGame()->AddCoin();
            other->GetOwner()->SetState(ActorState::Destroy);
        }
    }
}

void Mario::OnVerticalCollision(const float minOverlap, AABBColliderComponent* other)
{
    auto* rbc = GetComponent<RigidBodyComponent>();
    if (!rbc) {
        return;
    }

    // Check for ground collision
    if (minOverlap > 0.0f) {
        mIsOnGround = true;

        if (other->GetLayer() == ColliderLayer::Enemy) {
            Goomba* goomba = dynamic_cast<Goomba*>(other->GetOwner());
            if (goomba) {
                goomba->setStomped(true);
            }
            other->GetOwner()->Kill();
            
            Vector2 vel = rbc->GetVelocity();
            vel.y = mJumpSpeed * 0.6f; 
            rbc->SetVelocity(vel);
            SetOffGround();

            // show stomp animation briefly
            auto* ac = GetComponent<AnimatorComponent>();
            if (ac) {
                ac->SetAnimation("stomp");
                mStompTimer = 0.40f;
            }
        }
    }
    // Check for hitting blocks/enemies from below
    else if (minOverlap < 0.0f) {
        if (other->GetLayer() == ColliderLayer::Blocks) {
            Block* block = dynamic_cast<Block*>(other->GetOwner());
            if (block) {
                std::string tex = block->GetTexturePath();
                
                // If it's a brick block ("BlockB.png")
                if (tex == "BlockB.png") {
                    block->Bump();
                    if (mIsSuper) {
                        // Super Mario breaks the block
                        block->SetState(ActorState::Destroy);
                    }
                }
                // If it's a question block ("BlockC.png"), it always bumps
                else if (tex == "BlockC.png") {
                    block->Bump();
                }
            }
        }
    }
    else if (other->GetLayer() == ColliderLayer::Enemy) {
        Kill();
    }

    // If colliding with a collectable
    if (other->GetLayer() == ColliderLayer::Collectable) {
        if (dynamic_cast<Mushroom*>(other->GetOwner())) {
            PowerUp();
            other->GetOwner()->Kill();
        }
        else if (dynamic_cast<Coin*>(other->GetOwner())) {
            GetGame()->AddCoin();
            other->GetOwner()->SetState(ActorState::Destroy);
        }
    }
}

void Mario::PowerUp()
{
    if (mIsSuper) return;
    mIsSuper = true;

    // Reset vertical velocity
    auto* rbc = GetComponent<RigidBodyComponent>();
    if (rbc) {
        Vector2 vel = rbc->GetVelocity();
        vel.y = -150.0f;
        rbc->SetVelocity(vel);
    }

    // Update sprite sheet for super Mario
    auto* ac = GetComponent<AnimatorComponent>();
    if (ac) {
        
        ac->SetSpriteSheet("../Assets/Sprites/SuperMario/SuperMario.png", "../Assets/Sprites/SuperMario/SuperMario.json");

        ac->AddAnimation("dead", {0});    // its not a super mario animation, but we keep it for consistency
        ac->AddAnimation("idle", {0});
        ac->AddAnimation("jump", {2});
        ac->AddAnimation("run", {6, 7, 8});
        ac->AddAnimation("run", {3, 4, 5});
        ac->AddAnimation("stomp", {0, 2});  
        ac->AddAnimation("turn", {0});
        ac->SetAnimFPS(10.0f);
        ac->SetAnimation("idle");

        ac->SetSize(Game::TILE_SIZE, Game::TILE_SIZE * 2.0f);
        ac->SetOffset(Vector2(0.0f, -Game::TILE_SIZE / 2.0f));
    }

    // Update collider size and offset
    auto* cc = GetComponent<AABBColliderComponent>();
    if (cc) {
        cc->SetSize(Game::TILE_SIZE, Game::TILE_SIZE * 2.0f);
        cc->SetOffset(Vector2(0.0f, -Game::TILE_SIZE / 2.0f));
    }
}

void Mario::PowerDown()
{
    if (!mIsSuper) return;
    mIsSuper = false;

    // Set invincibility timer
    mInvincibilityTimer = 1.5f;

    // Update animations based on state
    auto* ac = GetComponent<AnimatorComponent>();
    if (ac) {
        
        ac->SetSpriteSheet("../Assets/Sprites/Mario/Mario.png", "../Assets/Sprites/Mario/Mario.json");

        ac->AddAnimation("dead", {0});
        ac->AddAnimation("idle", {1});
        ac->AddAnimation("jump", {2});
        ac->AddAnimation("run", {3, 4, 5});
        ac->AddAnimation("stomp", {6, 7});  
        ac->AddAnimation("turn", {8});
        ac->SetAnimFPS(10.0f);
        ac->SetAnimation("idle");

        ac->SetSize(Game::TILE_SIZE, Game::TILE_SIZE);
        ac->SetOffset(Vector2::Zero);
    }

    // Update collider size and offset
    auto* cc = GetComponent<AABBColliderComponent>();
    if (cc) {
        cc->SetSize(Game::TILE_SIZE, Game::TILE_SIZE);
        cc->SetOffset(Vector2::Zero);
    }
}