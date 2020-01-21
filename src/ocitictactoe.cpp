
/**
 *  @file
 *  @copyright defined in eos/LICENSE.txt
 */

//#include <eosio.system/eosio.system.hpp>

#include <eosiolib/eosio.hpp>
#include <eosiolib/print.hpp>



#include <ocitictactoe.hpp>

using namespace eosio;


   /**
    * @brief Check if cell is empty
    * @param cell - value of the cell (should be either 0, 1, or 2)
    * @return true if cell is empty
    */
   bool ocitictactoe::is_empty_cell(uint8_t cell) {
      return cell == 0;
   }

   /**
    * @brief Check for valid movement
    * @detail Movement is considered valid if it is inside the board and done on empty cell
    * @param mvt - the movement made by the player
    * @param game_for_movement - the game on which the movement is being made
    * @return true if movement is valid
    */
   bool ocitictactoe::is_valid_movement(uint32_t row, uint32_t column, const game& game_for_movement) {
      uint32_t cell = row * 3 + column;
      return (row < 3 && column < 3) && is_empty_cell(game_for_movement.board[cell]);
   }

   /**
    * @brief Get winner of the game
    * @detail Winner of the game is the first player who made three consecutive aligned movement
    * @param current_game - the game which we want to determine the winner of
    * @return winner of the game (can be either none/ draw/ account name of host/ account name of challenger)
    */
   name ocitictactoe::get_winner(const game& current_game) {
      if((current_game.board[0] == current_game.board[4] && current_game.board[4] == current_game.board[8]) ||
         (current_game.board[1] == current_game.board[4] && current_game.board[4] == current_game.board[7]) ||
         (current_game.board[2] == current_game.board[4] && current_game.board[4] == current_game.board[6]) ||
         (current_game.board[3] == current_game.board[4] && current_game.board[4] == current_game.board[5])) {
         //  x | - | -    - | x | -    - | - | x    - | - | -
         //  - | x | -    - | x | -    - | x | -    x | x | x
         //  - | - | x    - | x | -    x | - | -    - | - | -
         if (current_game.board[4] == 1) {
            return current_game.host;
         } else if (current_game.board[4] == 2) {
            return current_game.challenger;
         }
      } else if ((current_game.board[0] == current_game.board[1] && current_game.board[1] == current_game.board[2]) ||
                 (current_game.board[0] == current_game.board[3] && current_game.board[3] == current_game.board[6])) {
         //  x | x | x       x | - | -
         //  - | - | -       x | - | -
         //  - | - | -       x | - | -
         if (current_game.board[0] == 1) {
            return current_game.host;
         } else if (current_game.board[0] == 2) {
            return current_game.challenger;
         }
      } else if ((current_game.board[2] == current_game.board[5] && current_game.board[5] == current_game.board[8]) ||
                 (current_game.board[6] == current_game.board[7] && current_game.board[7] == current_game.board[8])) {
         //  - | - | x       - | - | -
         //  - | - | x       - | - | -
         //  - | - | x       x | x | x
         if (current_game.board[8] == 1) {
            return current_game.host;
         } else if (current_game.board[8] == 2) {
            return current_game.challenger;
         }
      } else {
         bool is_board_full = true;
         for (uint8_t i = 0; i < board_len; i++) {
            if (is_empty_cell(current_game.board[i])) {
               is_board_full = false;
               break;
            }
         }
         if (is_board_full) {
            return "draw"_n;
         }
      }
      return "none"_n;
   }

   /**
    * @brief Apply create action
    * @param c - action to be applied
    */
    void ocitictactoe::create(name host, name challenger) {
      #if 0
      permission_level perm({host,"gamemaster"_n});
      require_auth(perm);
      #else 
      require_auth(host);
      #endif
      eosio_assert(challenger != host, "challenger same as host");

      // Check if game already exists
      game_index existing_host_games(_code, host.value );
      auto itr = existing_host_games.find( challenger.value );
      eosio_assert(itr == existing_host_games.end(), "game already exists");

      existing_host_games.emplace(host, [&]( auto& g ) {
         g.challenger = challenger;
         g.host = host;
         g.turn = host;
      });
   }

   /**
    * @brief Apply restart action
    * @param r - action to be applied
    */
    void ocitictactoe::restart(name host, name challenger, name by) {
      require_auth(by);

      // Check if game exists
      game_index existing_host_games(_code, host.value);
      auto itr = existing_host_games.find( challenger.value );
      eosio_assert(itr != existing_host_games.end(), "game doesn't exists");

      // Check if this game belongs to the action sender
      eosio_assert(by == itr->host || by == itr->challenger, "this is not your game!");

      // Reset game
      existing_host_games.modify(itr, itr->host, []( auto& g ) {
         g.reset_game();
      });
   }

   /**
    * @brief Apply close action
    * @param c - action to be applied
    */
    void ocitictactoe::erase(name host, name challenger) {
      require_auth(host);

      // Check if game exists
      game_index existing_host_games(_code, host.value);
      auto itr = existing_host_games.find( challenger.value );
      eosio_assert(itr != existing_host_games.end(), "game doesn't exists");

      // Remove game
      existing_host_games.erase(itr);
   }

   /**
    * @brief Apply move action
    * @param m - action to be applied
    */
      void 
      ocitictactoe::move (name host, name challenger, name by, uint32_t row, uint32_t column) {
      print("TTT action: move ", host, " vs ", challenger," by ", by, "to ", row, " x ", column, "\n");
      require_auth(by);

      // Check if game exists
      game_index existing_host_games(_code, host.value);
      auto itr = existing_host_games.find( challenger.value );
      eosio_assert(itr != existing_host_games.end(), "no matching game");

      // Check if this game hasn't ended yet
      eosio_assert(itr->winner == "none"_n, "the game has ended!");
      // Check if this game belongs to the action sender
      eosio_assert(by == itr->host || by == itr->challenger, "this is not your game!");
      // Check if this is the  action sender's turn
      eosio_assert(by == itr->turn, "it's not your turn yet!");

      // Check if user makes a valid movement
      eosio_assert(is_valid_movement(row, column, *itr), "not a valid movement!");

      // Fill the cell, 1 for host, 2 for challenger
      const uint8_t cell_value = itr->turn == itr->host ? 1 : 2;
      const auto turn = itr->turn == itr->host ? itr->challenger : itr->host;
      existing_host_games.modify(itr, itr->host, [&]( auto& g ) {
         g.board[row * 3 + column] = cell_value;
         g.turn = turn;

         //check to see if we have a winner
         g.winner = get_winner(g);
      });
   }

EOSIO_DISPATCH( ocitictactoe, (create)(restart)(erase)(move))
