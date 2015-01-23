#ifndef SDQ_GAME_PARAMS_HPP
#define SDQ_GAME_PARAMS_HPP

enum icon_t { nothing, left, right, up, down, button };

const icon_t lvl_bats          [] = {right,right,right,left,button,button,
                                     button};
const icon_t lvl_totem         [] = {left,right,left,right,down,up};
const icon_t lvl_fire_woman    [] = {right,left,up,button,up,button};
const icon_t lvl_pyramid_steps [] = {button,button,button,right,button,button,
                                     right,button};
const icon_t lvl_water_lift    [] = {left};
const icon_t lvl_serpents      [] = {button,button,button,right,left,button,
                                     button,down};
const icon_t lvl_mummy         [] = {right,up,button,right,button};
const icon_t lvl_gulley        [] = {down,left,right,left,left,left};
const icon_t lvl_skeletons     [] = {right,button,button,button,button,right,
                                     button,button,button,button,button,button,
                                     button,right,button,button};
const icon_t lvl_hands         [] = {right,up,button,left,button,button,left,
                                     button,button,button,button};
const icon_t lvl_snake         [] = {left,up,up,left,up,left,right};
const icon_t lvl_dragon        [] = {right,left,down,right,up,left,left,left,
                                     up,right,button};
const icon_t lvl_jellyfish     [] = {button,button,button,button,button,button,
                                     button,button,up,button};
const icon_t lvl_river_jump    [] = {down,up,right,down,right,up};
const icon_t lvl_river_logs    [] = {right,left,right,left,left,up,down,left,
                                     right,up};
const icon_t lvl_river_raft    [] = {right,right,up,right,left,left,down,right};
const icon_t lvl_windmill      [] = {left,up,button,right,up};
const icon_t lvl_chariots      [] = {up,up,up,up,up,up};
const icon_t lvl_stair_chute   [] = {left};
const icon_t lvl_closing_walls [] = {left};
const icon_t lvl_winged_goblins[] = {button,button};
const icon_t lvl_laser_eyes[]     = {left,right,up,left};
const icon_t lvl_witch[]          = {right,button,left,up,right,left,button,
                                     button,right,left,button};

#endif // SDQ_GAME_PARAMS_HPP
