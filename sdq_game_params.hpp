#ifndef SDQ_GAME_PARAMS_HPP
#define SDQ_GAME_PARAMS_HPP

#include <vector>
#include <forward_list>

enum prompt_e { nothing, L, R, U, D, X };
enum level_e  { bats, totem, fire_woman, pyramid_steps, water_lift, serpents,
                mummy, gulley, skeletons, hands, snake, dragon, jellyfish,
                river_jump, river_logs, river_raft, windmill, chariots,
                stair_chute, closing_walls, winged_goblins, laser_eyes,
                witch, num_levels };

struct sdq_moves {

  sdq_moves()
  {
    level_moves[ bats ]           = {R,R,R,L,X,X,X};
    level_moves[ totem ]          = {L,R,L,R,D,U};
    level_moves[ fire_woman ]     = {R,L,U,X,U,X};
    level_moves[ pyramid_steps ]  = {X,X,X,R,X,X,R,X};
    level_moves[ water_lift ]     = {L};
    level_moves[ serpents ]       = {X,X,X,R,L,X,X,D};
    level_moves[ mummy ]          = {R,U,X,R,X};
    level_moves[ gulley ]         = {D,L,R,L,L,L};
    level_moves[ skeletons ]      = {R,X,X,X,X,R,X,X,X,X,X,X,X,R,X,X};
    level_moves[ hands ]          = {R,U,X,L,X,X,L,X,X,X,X};
    level_moves[ snake ]          = {L,U,U,L,U,L,R};
    level_moves[ dragon ]         = {R,L,D,R,U,L,L,L,U,R,X};
    level_moves[ jellyfish ]      = {X,X,X,X,X,X,X,X,U,X};
    level_moves[ river_jump ]     = {D,U,R,D,R,U};
    level_moves[ river_logs ]     = {R,L,R,L,L,U,D,L,R,U};
    level_moves[ river_raft ]     = {R,R,U,R,L,L,D,R};
    level_moves[ windmill ]       = {L,U,X,R,U};
    level_moves[ chariots ]       = {U,U,U,U,U,U};
    level_moves[ stair_chute ]    = {L};
    level_moves[ closing_walls ]  = {U};
    level_moves[ winged_goblins ] = {X,X};
    level_moves[ laser_eyes ]     = {L,R,U,L};
    level_moves[ witch ]          = {R,X,L,U,R,L,X,X,R,L,X};
  }

  using type = std::vector<prompt_e>;
  type level_moves[level_e::num_levels];
};

struct sdq_moves_exhaustive {

  sdq_moves_exhaustive(const sdq_moves &move_bank) {

    for (unsigned i = 0; i < level_e::num_levels; ++i) {
      for (const prompt_e &p : move_bank.level_moves[i]) {
        auto comp = [&p](const prompt_e &, const prompt_e &b) { return b==p; };
        std::forward_list<prompt_e> ps{L,R,U,D,X};
        ps.sort(comp);                     // p is now in last position
        moves[i].push_back(ps);
      }
    }
  }

  using type = std::vector<std::forward_list<prompt_e>>;
  type moves[level_e::num_levels];
};

#endif // SDQ_GAME_PARAMS_HPP
