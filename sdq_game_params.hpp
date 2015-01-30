#ifndef SDQ_GAME_PARAMS_HPP
#define SDQ_GAME_PARAMS_HPP

#include <tuple>

enum prompt_t  { nothing, L, R, U, D, X };
enum level_t { bats, totem, fire_woman, pyramid_steps, water_lift, serpents,
               mummy, gulley, skeletons, hands, snake, dragon, jellyfish,
               river_jump, river_logs, river_raft, windmill, chariots,
               stair_chute, closing_walls, winged_goblins, laser_eyes, witch };

const prompt_t level_bats          [] = {R,R,R,L,X,X,X};

template <prompt_t...>
  struct prompts {};

template <level_t,typename>
  struct num_prompts;

template <>
struct num_prompts<bats,prompts<R,L>> {
  static const unsigned value = sizeof(level_bats)/sizeof(prompt_t);
};

const prompt_t level_totem         [] = {L,R,L,R,D,U};
const prompt_t level_fire_woman    [] = {R,L,U,X,U,X};
const prompt_t level_pyramid_steps [] = {X,X,X,R,X,X,R,X};
const prompt_t level_water_lift    [] = {L};
const prompt_t level_serpents      [] = {X,X,X,R,L,X,X,D};
const prompt_t level_mummy         [] = {R,U,X,R,X};
const prompt_t level_gulley        [] = {D,L,R,L,L,L};
const prompt_t level_skeletons     [] = {R,X,X,X,X,R,X,X,X,X,X,X,X,R,X,X};
const prompt_t level_hands         [] = {R,U,X,L,X,X,L,X,X,X,X};
const prompt_t level_snake         [] = {L,U,U,L,U,L,R};
const prompt_t level_dragon        [] = {R,L,D,R,U,L,L,L,U,R,X};
const prompt_t level_jellyfish     [] = {X,X,X,X,X,X,X,X,U,X};
const prompt_t level_river_jump    [] = {D,U,R,D,R,U};
const prompt_t level_river_logs    [] = {R,L,R,L,L,U,D,L,R,U};
const prompt_t level_river_raft    [] = {R,R,U,R,L,L,D,R};
const prompt_t level_windmill      [] = {L,U,X,R,U};
const prompt_t level_chariots      [] = {U,U,U,U,U,U};
const prompt_t level_stair_chute   [] = {L};
const prompt_t level_closing_walls [] = {L};
const prompt_t level_winged_goblins[] = {X,X};
const prompt_t level_laser_eyes    [] = {L,R,U,L};
const prompt_t level_witch         [] = {R,X,L,U,R,L,X,X,R,L,X};

static_assert(sizeof(prompt_t)==4,"");
static_assert(sizeof(level_bats)==7*sizeof(prompt_t),"");

#endif // SDQ_GAME_PARAMS_HPP
