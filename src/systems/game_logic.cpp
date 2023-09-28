#include "game_logic.h"
#include "../components/input.h"
#include "../components/inventory.h"
#include "../components/mountain.h"
#include "../components/player.h"
#include "../components/render_components.h"
#include "iostream"
#include "physics.h"
#include "raylib.h"
#include "render_systems.h"
#include "rockSpawnStuff.h"
#include <cmath>

float item_spawn_time = 0;

void moveKillBar(flecs::iter it, KillBar *killBar) {
    killBar->x += it.delta_time() * KILL_BAR_VELOCITY;
}

void checkPlayerAlive(flecs::iter iter, Position *position, KillBar *killBar) {
    // TODO multiplayer
    // TODO rename and/or combine with checkPlayerIsHit
    if (position[0].x < killBar->x) {
        std::cout << "Player Dead" << std::endl;
        iter.world().get_mut<AppInfo>()->playerAlive = false;
    }
}

// TODO: remove this if not needed
void debugRenderPlayer(flecs::iter it, Position *position, KillBar *killBar) {
    Vector2 pos{position[0].x, position[0].y};

    BeginDrawing();
    auto camera =
        it.world().lookup("Camera").get<graphics::Camera2DComponent>();
    BeginMode2D(*camera);
    Vector2 start{killBar->x, 0.};
    Vector2 stop{killBar->x, 1000.};

    DrawLineV(start, stop, RED);

    DrawCircleV(pos, 20., RED);
    EndMode2D();
    EndDrawing();
}

void moveCamera(flecs::iter it, Position *position, KillBar *killBar,
                Mountain *mountain) {
    auto camera =
        it.world().lookup("Camera").get_mut<graphics::Camera2DComponent>();
    camera->target.x = (killBar->x) + (graphics::SCREEN_WIDTH * 1.0f) / 2;
    // fix camera to y-coord of player

    // smoothed movement in y-direction
    constexpr int camera_smoothing{
        50}; // the higher this number the more aggressively it gets smoothed
    float current_camera_pos = camera->target.y;
    float target_camera_pos = -position[0].y;
    float diff = target_camera_pos - current_camera_pos;
    camera->target.y =
        ((current_camera_pos) * (camera_smoothing - 1) + target_camera_pos) /
        camera_smoothing;

    // //not smoothed movement in y-direction
    // camera->target.y = -position[0].y;
}

/**
 * Checks if a new chunk should be generated and tells the mountain to generate
 * a new chunk if needed
 * @param it
 * @param mountain
 * @param killBar
 */
void chunkSystem(flecs::iter it, Mountain *mountain, KillBar *killBar) {
    float current_left_edge_screen{killBar->x};
    float leftest_point_of_mountain{
        mountain
            ->getVertex(
                mountain->getIndexIntervalOfEntireMountain().start_index)
            .x};
    constexpr float CHUNK_DESTROY_BUFFER_CONSTANT{1.0};
    // std::cout << "Position killbar: " << killBar->x << " left point
    // mountain:"
    //<< leftest_point_of_mountain << std::endl;
    if (leftest_point_of_mountain < current_left_edge_screen -
                                        Mountain::CHUNK_WIDTH -
                                        CHUNK_DESTROY_BUFFER_CONSTANT) {
        // leftest_point_of_mountain =
        // mountain->getVertex(mountain->getIndexIntervalOfEntireMountain().start_index).x;
        std::cout << "Position killbar: " << killBar->x
                  << " left point mountain: " << leftest_point_of_mountain
                  << std::endl;
        if (it.world().get<AppInfo>()->playerAlive) {
            mountain->generateNewChunk();
            graphics::generateChunkMesh(it.world());
        }
        // flecs::entity e = it.world().entity("tmp_event");
        // it.world()
        //     .event<graphics::GenChunkEvent>()
        //     .id<Mountain>()
        //     .entity(e)
        //     .emit();
        //  std::cout << "chunk generated" << std::endl;
    }
}

void spawnRocks(flecs::iter it, Mountain *mountain,
                rockSpawnStuff::SpawnData *spawnData) {
    auto camera =
        it.world().lookup("Camera").get_mut<graphics::Camera2DComponent>();

    auto gameTime = GetTime();
    rockSpawnStuff::RockSpawnPhase rockSpawnPhase =
        rockSpawnStuff::determineRockSpawnPhase(gameTime);
    float time_between_rockspawns =
        rockSpawnStuff::rockSpawnTimeFromPhase(rockSpawnPhase);

    if (gameTime > spawnData->rock_spawn_time + time_between_rockspawns) {
        spawnData->rock_spawn_time =
            spawnData->rock_spawn_time + time_between_rockspawns;

        // compute spawn Basepoint
        // offset on x-axis by -300 to spawn visible on screen
        constexpr float DEBUG_MAKE_SPAWN_VISIBLE_OFFSET = 400.;

        const float spawn_x_coord = camera->target.x +
                                    ((float)graphics::SCREEN_WIDTH) / 2 +
                                    100; //-DEBUG_MAKE_SPAWN_VISIBLE_OFFSET;
        Position spawnBasepoint = mountain->getVertex(
            mountain->getRelevantMountainSection(spawn_x_coord, spawn_x_coord)
                .start_index);
        // spawn rocks offset by constant amount above mountain
        spawnBasepoint.y += 350.;

        int num_rocks_to_spawn =
            rockSpawnStuff::computeNumRocksToSpawn(rockSpawnPhase, spawnData);
        const std::vector<Position> offsets_additional_rocks{
            {0., 0.},
            {MAX_ROCK_SIZE + 5., MAX_ROCK_SIZE * 2 + 10.},
            {-MAX_ROCK_SIZE - 5., MAX_ROCK_SIZE * 2 + 10.}};

        for (int i{0}; i < num_rocks_to_spawn; i++) {
            double r = ((double)std::rand() / (RAND_MAX));
            float radius =
                ((float)r) * (MAX_ROCK_SIZE - MIN_ROCK_SIZE) + MIN_ROCK_SIZE;

            constexpr float CONST_VEL_COMPONENT{-300.};
            float random_vel_component{
                ((((float)std::rand()) / RAND_MAX) - 0.5f) * 200.f};

            std::cout << "rock spawned" << std::endl;
            auto rock_entity =
                it.world()
                    .entity()
                    .set<Position>(
                        {spawnBasepoint.x + offsets_additional_rocks[i].x,
                         spawnBasepoint.y + offsets_additional_rocks[i].y})
                    .set<Velocity>(
                        {CONST_VEL_COMPONENT + random_vel_component, 0})
                    .set<Radius>({radius})
                    .set<Rotation>({0, 0})
                    .add<Rock>()
                    //.add<Exploding>()
                    //.set<graphics::CircleShapeRenderComponent>({radius});
                    .set([&](graphics::BillboardComponent &c) {
                        c = {0};
                        c.billUp = {0.0f, 0.0f, 1.0f};
                        c.billPositionStatic = {-radius / 2, 0.0f, -radius / 2};
                        c.resourceHandle =
                            it.world()
                                .get_mut<graphics::Resources>()
                                ->textures.load("../assets/texture/stone.png");
                        c.width = radius * 2.0f;
                        c.height = radius * 2.0f;
                    });

            if (rockSpawnPhase == rockSpawnStuff::explosiveBatches) {
                spawnData->explosive_rock_modulo_count++;
                if (spawnData->explosive_rock_modulo_count >= 10) {
                    spawnData->explosive_rock_modulo_count = 0;
                    rock_entity.add<Exploding>();
                }
            }
        }
    }
}

void mountainLoadChunks(const flecs::world &world) {
    Mountain *mountain = world.get_mut<Mountain>();
    for (std::size_t i{0};
         i < Mountain::NUMBER_OF_VERTICES / Mountain::NUM_SECTIONS_PER_CHUNK;
         i++) {
        mountain->generateNewChunk();
        graphics::generateChunkMesh(world);
    }
}

void updateScore(flecs::iter it, Position *position, AppInfo *appInfo) {
    appInfo->score = std::max(appInfo->score, (int)position[0].x);
    // std::cout << "Score: " << appInfo->score << std::endl;
}

void spawnExplosion(flecs::world &world, Position pos) {
    world.entity().set<Position>({pos}).set<graphics::LifeTime>({10}).set(
        [&](graphics::AnimatedBillboardComponent &c) {
            c = {0};
            c.billUp = {0.0f, 0.0f, 1.0f};
            c.billPositionStatic = {0.0f, 0.0f, 0.0};
            c.resourceHandle =
                world.get_mut<graphics::Resources>()->textures.load(
                    "../assets/texture/explosion.png");
            c.width = 200; // TODO?
            c.height = 200;
            c.current_frame = 0;
            c.animation_speed = 1;
            c.numFrames = 25;
        });
}

void removeSFX(flecs::iter it, graphics::LifeTime *lifetime) {
    for (auto i : it) {
        lifetime[i].time -= it.delta_time();
        if (lifetime[i].time < 0) {
            it.entity(i).destruct();
        }
    }
}

// debug function
// void countRocks(flecs::iter it, Rock* rocks){
//     int acc{0};
//     for(auto i: it){
//         acc++;
//     }
//     std::cout<<"rock count: " << acc << std::endl;
// }

// generate a random variable that follows the geometric distribution with p =
// 1/2^p_exp
int randGeometric(int p_exp) {
    int rand = RAND_MAX;
    for (int i = 0; i < p_exp; ++i) {
        rand = rand & std::rand();
    }
    return std::floor(std::log(RAND_MAX) - std::log(rand));
}

void spawnItems(flecs::iter it) {
    if ((GetTime() - item_spawn_time) > 0) {
        auto type = (ItemClass::Items)(rand() % ItemClass::Items::ITEM_COUNT);
        float_type x_position = it.world()
                                    .lookup("Camera")
                                    .get_mut<graphics::Camera2DComponent>()
                                    ->target.x +
                                (float)graphics::SCREEN_WIDTH / 2;
        auto position =
            Position{x_position, physics::getYPosFromX(it.world(), x_position)};
        position.y += ITEM_BASE_HEIGHT + (ITEM_MAX_HEIGHT - ITEM_BASE_HEIGHT) *
                                             ((float)std::rand() / RAND_MAX);
        item_spawn_time =
            GetTime() + 3. + (.125 / ITEMS_PER_SECOND * randGeometric(3));

        std::cout << "spawning " << ITEM_CLASSES[type].name << " at "
                  << position.x << "," << position.y << " " << std::endl;
        it.world()
            .entity()
            .set<Position>(position)
            .set<Item>({type})
            .set<graphics::BillboardComponent>(
                {.billUp = {0.0f, 0.0f, 1.0f},
                 .billPositionStatic = {0.0f, 0.0f, 0.0f},
                 .width = 50,
                 .height = 50,
                 .resourceHandle =
                     it.world().get_mut<graphics::Resources>()->textures.load(
                         ITEM_CLASSES[type].texture)});
    }
}

void initGameLogic(flecs::world &world) {
    mountainLoadChunks(world);

    auto player_0_input = InputEntity();
    // spawnExplosion(world);
    world.entity()
        .add<Player>()
        .set<Position>({200., physics::getYPosFromX(world, 200.)})
        .set<Velocity>({0., 0.})
        .set<PlayerMovement>({PlayerMovement::MovementState::IDLE,
                              PlayerMovement::Direction::NEUTRAL, true, 0})
        .set<Height>({HIKER_HEIGHT})
        .set<Width>({HIKER_WIDTH})
        .set<InputEntity>(player_0_input)
        .set<Health>({HIKER_MAX_HEALTH})
        .set<InteractionRadius>({HIKER_ITEM_COLLECTION_RANGE})
        .set<Inventory>(Inventory{INVENTORY_SLOTS})
        .set([&](graphics::AnimatedBillboardComponent &c) {
            c = {0};
            c.billUp = {0.0f, 0.0f, 1.0f};
            c.billPositionStatic = {0.0f, 0.0f, -HIKER_HEIGHT / 2};
            c.resourceHandle =
                world.get_mut<graphics::Resources>()->textures.load(
                    "../assets/texture/player_walk.png");
            c.width = HIKER_HEIGHT; // TODO?
            c.height = HIKER_HEIGHT;
            c.current_frame = 0;
            c.animation_speed = 1;
            c.numFrames = 4;
        })
        .set<graphics::RectangleShapeRenderComponent>({
            HIKER_WIDTH,HIKER_HEIGHT
        });

    auto can_collect_system = world.system<Position, InteractionRadius>()
                                  .kind(flecs::PreUpdate)
                                  .iter(Inventory::checkCanCollect);

    world.system<InputEntity, Inventory>()
        .kind(flecs::PreUpdate)
        .iter(Inventory::updateInventory)
        .depends_on(can_collect_system);

    world.system<graphics::LifeTime>().kind(flecs::PreUpdate).iter(removeSFX);

    world.set<KillBar>({0.});

    world.system<KillBar>().term_at(1).singleton().iter(moveKillBar);

    world.system<Position, KillBar>()
        .with<Player>()
        .term_at(2)
        .singleton()
        .iter(checkPlayerAlive);

    world.system<Position>().with<Item>().iter(physics::checkRockInScope);

    world.system<Mountain, KillBar>()
        .term_at(1)
        .singleton()
        .term_at(2)
        .singleton()
        .iter(chunkSystem);

    world.system<Position, KillBar, Mountain>()
        .with<Player>()
        .term_at(2)
        .singleton()
        .term_at(3)
        .singleton()
        .iter(moveCamera);

    world.system<Position, AppInfo>()
        .with<Player>()
        .term_at(2)
        .singleton()
        .iter(updateScore);

    world.system<>().iter(spawnItems);
    world.set<rockSpawnStuff::SpawnData>({});
    world.system<Mountain, rockSpawnStuff::SpawnData>()
        .term_at(1)
        .singleton()
        .term_at(2)
        .singleton()
        .iter(spawnRocks);
}
