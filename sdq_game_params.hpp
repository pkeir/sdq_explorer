#ifndef SDQ_GAME_PARAMS_HPP
#define SDQ_GAME_PARAMS_HPP

#include <utility>
using std::integer_sequence; // C++14

enum prompt_t   { nothing, L, R, U, D, X };
enum level_id_t { bats, totem, fire_woman, pyramid_steps, water_lift, serpents,
                  mummy, gulley, skeletons, hands, snake, dragon, jellyfish,
                  river_jump, river_logs, river_raft, windmill, chariots,
                  stair_chute, closing_walls, winged_goblins, laser_eyes,
                  witch };

static_assert(sizeof(level_id_t)==4,"");

using bats_t       = integer_sequence<prompt_t,R,R,R,L,X,X,X>;
using totem_t      = integer_sequence<prompt_t,L,R,L,R,D,U>;
using fire_woman_t = integer_sequence<prompt_t,R,L,U,X,U,X>;
using pyramid_steps_t  = integer_sequence<prompt_t,X,X,X,R,X,X,R,X>;
using water_lift_t = integer_sequence<prompt_t,L>;
using serpents_t   = integer_sequence<prompt_t,X,X,X,R,L,X,X,D>;
using mummy_t      = integer_sequence<prompt_t,R,U,X,R,X>;
using gulley_t     = integer_sequence<prompt_t,D,L,R,L,L,L>;
using skeletons_t  = integer_sequence<prompt_t,R,X,X,X,X,R,X,X,X,X,X,X,X,R,X,X>;
using hands_t      = integer_sequence<prompt_t,R,U,X,L,X,X,L,X,X,X,X>;
using snake_t      = integer_sequence<prompt_t,L,U,U,L,U,L,R>;
using dragon_t     = integer_sequence<prompt_t,R,L,D,R,U,L,L,L,U,R,X>;
using jellyfish_t  = integer_sequence<prompt_t,X,X,X,X,X,X,X,X,U,X>;
using river_jump_t = integer_sequence<prompt_t,D,U,R,D,R,U>;
using river_logs_t = integer_sequence<prompt_t,R,L,R,L,L,U,D,L,R,U>;
using river_raft_t = integer_sequence<prompt_t,R,R,U,R,L,L,D,R>;
using windmill_t   = integer_sequence<prompt_t,L,U,X,R,U>;
using chariots_t   = integer_sequence<prompt_t,U,U,U,U,U,U>;
using stair_chute_t    = integer_sequence<prompt_t,L>;
using closing_walls_t  = integer_sequence<prompt_t,L>;
using winged_goblins_t = integer_sequence<prompt_t,X,X>;
using laser_eyes_t = integer_sequence<prompt_t,L,R,U,L>;
using witch_t      = integer_sequence<prompt_t,R,X,L,U,R,L,X,X,R,L,X>;

template <level_id_t>
  struct level_t;

template <> struct level_t<bats>           { using type = bats_t; };
template <> struct level_t<totem>          { using type = totem_t; };
template <> struct level_t<fire_woman>     { using type = fire_woman_t; };
template <> struct level_t<pyramid_steps>  { using type = pyramid_steps_t; };
template <> struct level_t<water_lift>     { using type = water_lift_t; };
template <> struct level_t<serpents>       { using type = serpents_t; };
template <> struct level_t<mummy>          { using type = mummy_t; };
template <> struct level_t<gulley>         { using type = gulley_t; };
template <> struct level_t<skeletons>      { using type = skeletons_t; };
template <> struct level_t<hands>          { using type = hands_t; };
template <> struct level_t<snake>          { using type = snake_t; };
template <> struct level_t<dragon>         { using type = dragon_t; };
template <> struct level_t<jellyfish>      { using type = jellyfish_t; };
template <> struct level_t<river_jump>     { using type = river_jump_t; };
template <> struct level_t<river_logs>     { using type = river_logs_t; };
template <> struct level_t<river_raft>     { using type = river_raft_t; };
template <> struct level_t<windmill>       { using type = windmill_t; };
template <> struct level_t<chariots>       { using type = chariots_t; };
template <> struct level_t<stair_chute>    { using type = stair_chute_t; };
template <> struct level_t<closing_walls>  { using type = closing_walls_t; };
template <> struct level_t<winged_goblins> { using type = winged_goblins_t; };
template <> struct level_t<laser_eyes>     { using type = laser_eyes_t; };
template <> struct level_t<witch>          { using type = witch_t; };

/*const prompt_t level_bats          [] = {R,R,R,L,X,X,X};
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
*/
#endif // SDQ_GAME_PARAMS_HPP
