/**
 *  @file
 *  @copyright defined in eos/LICENSE.txt
 */
#include <eosiolib/eosio.hpp>
#include <eosiolib/contract.hpp>
/**
 *  @defgroup tictactoecontract Tic Tac Toe Contract
 *  @brief Defines the PvP tic tac toe contract example
 *  @ingroup examplecontract
 *
 *  @details
 *  For the following tic-tac-toe game:
 *  - Each pair of player can have 2 unique game, one where player_1 become host and player_2 become challenger and vice versa
 *  - The game data is stored in the "host" scope and use the "challenger" as the key
 *
 *  (0,0) coordinate is on the top left corner of the board
 *  @code
 *                 (0,2)
 *  (0,0) -  |  o  |  x      where - = empty cell
 *        -  |  x  |  -            x = move by host
 *  (2,0) x  |  o  |  o            o = move by challenger
 *  @endcode
 *
 *  Board is represented with number:
 *  - 0 represents empty cell
 *  - 1 represents cell filled by host
 *  - 2 represents cell filled by challenger
 *  Therefore, assuming x is host, the above board will have the following representation: [0, 2, 1, 0, 1, 0, 1, 2, 2] inside the game object
 *
 *  In order to deploy this contract:
 *  - Create an account called tic.tac.toe
 *  - Add tic.tac.toe key to your wallet
 *  - Set the contract on the tic.tac.toe account
 *
 *  How to play the game:
 *  - Create a game using `create` action, with you as the host and other account as the challenger.
 *  - The first move needs to be done by the host, use the `move` action to make a move by specifying which row and column to fill.
 *  - Then ask the challenger to make a move, after that it's back to the host turn again, repeat until the winner is determined.
 *  - If you want to restart the game, use the `restart` action
 *  - If you want to clear the game from the database to save up some space after the game has ended, use the `close` action
 *  @{
 */
 
 using namespace eosio;

class [[eosio::contract("ocitictactoe")]] ocitictactoe : public contract {
 public:
   using contract::contract;

   /**
    * @brief Data structure to hold game information
    */
 private:
   static const uint32_t board_len = 9;
   struct [[eosio::table]] game {
      game() { initialize_board(); }
      game(eosio::name challenger_account, eosio::name host_account)
            : challenger(challenger_account), host(host_account), turn(host_account) {
         // Initialize board
         initialize_board();
      }
      eosio::name     challenger;
      eosio::name     host;
      eosio::name     turn; // = account name of host/ challenger
      eosio::name     winner = "none"_n; // = none/ draw/ account name of host/ challenger
      std::vector<uint8_t>  board;

      // Initialize board with empty cell
      void initialize_board() {
        board.clear();
         for (uint8_t i = 0; i < board_len ; i++) {
            board.push_back(0);
         }
      }

      // Reset game
      void reset_game() {
         initialize_board();
         turn = host;
         winner = "none"_n;
      }

      uint64_t primary_key() const { return challenger.value; }

      EOSLIB_SERIALIZE( game, (challenger)(host)(turn)(winner)(board) )
   };

   typedef multi_index< "games"_n, game > game_index;

   bool is_empty_cell(uint8_t cell);

   /**
    * @brief Check for valid movement
    * @detail Movement is considered valid if it is inside the board and done on empty cell
    * @param mvt - the movement made by the player
    * @param game_for_movement - the game on which the movement is being made
    * @return true if movement is valid
    */
   bool is_valid_movement(uint32_t row, uint32_t column, const game& game_for_movement);


   /**
    * @brief Get winner of the game
    * @detail Winner of the game is the first player who made three consecutive aligned movement
    * @param current_game - the game which we want to determine the winner of
    * @return winner of the game (can be either none/ draw/ account name of host/ account name of challenger)
    */
   name get_winner(const game& current_game);

 public:
   /**
    * @brief Apply create action
    * @param c - action to be applied
    */
   [[eosio::action]]
      void create(name host, name challenger);

   /**
    * @brief Apply restart action
    * @param r - action to be applied
    */
   [[eosio::action]]
      void restart(name host, name challenger, name by);

   /**
    * @brief Apply close action
    * @param c - action to be applied
    */
   [[eosio::action]]
      void erase(name host, name challenger);
 
   /**
    * @brief Apply move action
    * @param m - action to be applied
    */
   [[eosio::action]]
      void move (name host, name challenger, name by, uint32_t row, uint32_t column);

/// @}
};
