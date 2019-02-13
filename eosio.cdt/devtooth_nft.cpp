#include "devtooth_nft.hpp"

namespace eosio {
    using std::string;
    using eosio::asset;

    ACTION devtooth_nft::create( name issuer, string sym ) {
	    require_auth( _self );

	    // Check if issuer account exists
	    eosio_assert( is_account( issuer ), "issuer account does not exist");

        // Valid symbol
        asset supply(0, symbol(symbol_code(sym), 0));

        auto symbol = supply.symbol;
        eosio_assert( symbol.is_valid(), "invalid symbol name" );
        eosio_assert( supply.is_valid(), "invalid supply");

        // Check if currency with symbol already exists
        currency_index currency_table( _self, symbol.code().raw() );
        auto existing_currency = currency_table.find( symbol.code().raw() );
        eosio_assert( existing_currency == currency_table.end(), "token with symbol already exists" );

        // Create new currency
        currency_table.emplace( _self, [&]( auto& currency ) {
           currency.supply = supply;
           currency.issuer = issuer;
        });
    }

    ACTION devtooth_nft::issue(name to, asset quantity, uint64_t index)
    {
	    eosio_assert( is_account( to ), "to account does not exist");

        // e,g, Get EOS from 3 EOS
        auto symbols = quantity.symbol;
        eosio_assert( symbols.is_valid(), "invalid symbol name" );
        eosio_assert( symbols.precision() == 0, "quantity must be a whole number" );

        // Ensure currency has been created
        auto symbol_name = symbols.code().raw();
        currency_index currency_table( _self, symbol_name );
        auto existing_currency = currency_table.find( symbol_name );
        eosio_assert( existing_currency != currency_table.end(), "token with symbol does not exist. create token before issue" );
        const auto& st = *existing_currency;

        // Ensure have issuer authorization and valid quantity
        //require_auth( to );
        require_auth(to);
        eosio_assert( quantity.is_valid(), "invalid quantity" );
        eosio_assert( quantity.amount > 0, "must issue positive quantity of uts" );
        eosio_assert( symbols == st.supply.symbol, "symbol precision mismatch" );

        // Case UTS
        if( symbols == symbol("UTS", 0)){
            // Get Servant info
            servant_table servant("unlimittest1"_n, to.value);
            auto servant_iter = servant.get(index, "Not exist Servant");

            // Check Token duplication
            auto uts_list = stokens.get_index<"byowner"_n>();
            bool not_exist = true;
            for(auto it = uts_list.begin(); it != uts_list.end(); ++it){
                if(it->t_idx == servant_iter.index){
                    not_exist = false;
                    break;
                }
            }
            eosio_assert(not_exist, "Already exist Token");

             // Add token with creator paying for RAM
            stokens.emplace( to, [&]( auto& token ) {
                token.idx = stokens.available_primary_key();
                token.t_idx = index;
                token.state = "idle";

                token.owner = to;
                token.master = to;
                token.value = asset{1, symbols};
            });
        }
        // Case UTM
        if( symbols == symbol("UTM", 0)){
            // Get Monster info
            monster_table monster("unlimittest1"_n, to.value);
            auto monster_iter = monster.get(index, "Not exist Monster");

            // Check Token duplication
            auto utm_list = mtokens.get_index<"byowner"_n>();
            bool not_exist = true;
            for(auto it = utm_list.begin(); it != utm_list.end(); ++it){
                if(it->t_idx == monster_iter.index){
                    not_exist = false;
                    break;
                }
            }
            eosio_assert(not_exist, "Already exist Token");

             // Add token with creator paying for RAM
            mtokens.emplace( to, [&]( auto& token ) {
                token.idx = mtokens.available_primary_key();
                token.t_idx = index;
                token.state = "idle";

                token.owner = to;
                token.master = to;
                token.value = asset{1, symbols};
            });
        }
        // Case UTI
        if( symbols == symbol("UTI", 0)){
            // Get Item info
            item_table item("unlimittest1"_n, to.value);
            auto item_iter = item.get(index, "Not exist Item");

            // Check Token duplication
            auto uti_list = itokens.get_index<"byowner"_n>();
            bool not_exist = true;
            for(auto it = uti_list.begin(); it != uti_list.end(); ++it){
                if(it->t_idx == item_iter.index){
                    not_exist = false;
                    break;
                }
            }
            eosio_assert(not_exist, "Already exist Token");

             // Add token with creator paying for RAM
            itokens.emplace( to, [&]( auto& token ) {
                token.idx = itokens.available_primary_key();
                token.t_idx = index;
                token.state = "idle";

                token.owner = to;
                token.master = to;
                token.value = asset{1, symbols};
            });
        }

        // Increase supply
	    add_supply( quantity );

        // Add balance to account
        add_balance( to, quantity, to );
    }

    ACTION devtooth_nft::transferid( name from, name to, id_type id, string sym)
    {
        // Ensure authorized to send from account
        eosio_assert( from != to, "cannot transfer to self" );
        require_auth( from );

        // Ensure 'to' account exists
        eosio_assert( is_account( to ), "to account does not exist");

        asset token(0, symbol(symbol_code(sym), 0));

        // UTS
        if( token.symbol == symbol("UTS", 0) ){
            auto sender_token = stokens.find( id );
            eosio_assert( sender_token != stokens.end(), "token with specified ID does not exist" );
            eosio_assert( sender_token->owner == from, "sender does not own token with specified ID");
            eosio_assert( sender_token->state == "idle", "a non-tradeable token");

            const auto& st = *sender_token;

            stokens.modify( st, from, [&]( auto& token ) {
	            token.owner = to;
            });

            sub_balance( from, st.value );
            add_balance( to, st.value, from );
        }
        // UTM
        if( token.symbol == symbol("UTM", 0) ){
            auto sender_token = mtokens.find( id );
            eosio_assert( sender_token != mtokens.end(), "token with specified ID does not exist" );
            eosio_assert( sender_token->owner == from, "sender does not own token with specified ID");
            eosio_assert( sender_token->state == "idle", "a non-tradeable token");

            const auto& st = *sender_token;

            mtokens.modify( st, from, [&]( auto& token ) {
	            token.owner = to;
            });

            sub_balance( from, st.value );
            add_balance( to, st.value, from );
        }
        // UTI
        if( token.symbol == symbol("UTI", 0) ){
            auto sender_token = itokens.find( id );
            eosio_assert( sender_token != itokens.end(), "token with specified ID does not exist" );
            eosio_assert( sender_token->owner == from, "sender does not own token with specified ID");
            eosio_assert( sender_token->state == "idle", "a non-tradeable token");

            const auto& st = *sender_token;

            itokens.modify( st, from, [&]( auto& token ) {
	            token.owner = to;
            });

            sub_balance( from, st.value );
            add_balance( to, st.value, from );
        }

	    // Notify both recipients
        require_recipient( from );
        require_recipient( to );
    }

    ACTION devtooth_nft::changestate(name from, string sym, id_type id){
        require_auth(from);

        asset token(0, symbol(symbol_code(sym), 0));

        // UTS
        if( token.symbol == symbol("UTS", 0) ){
            auto target_token = stokens.find( id );
            eosio_assert( target_token != stokens.end(), "token with specified ID does not exist" );
            eosio_assert( target_token->owner == from, "sender does not own token with specified ID");

            const auto& st = *target_token;

            if(st.state == "idle"){
                stokens.modify( st, from, [&]( auto& token ) {
	            token.state = "selling";
                });
            }
            else{
                stokens.modify( st, from, [&]( auto& token ) {
	            token.state = "idle";
                });
            }
        }
        // UTM
        if( token.symbol == symbol("UTM", 0) ){
            auto target_token = mtokens.find( id );
            eosio_assert( target_token != mtokens.end(), "token with specified ID does not exist" );
            eosio_assert( target_token->owner == from, "sender does not own token with specified ID");

            const auto& st = *target_token;

            if(st.state == "idle"){
                mtokens.modify( st, from, [&]( auto& token ) {
	            token.state = "selling";
                });
            }
            else{
                mtokens.modify( st, from, [&]( auto& token ) {
	            token.state = "idle";
                });
            }
        }
        // UTI
        if( token.symbol == symbol("UTI", 0) ){
            auto target_token = itokens.find( id );
            eosio_assert( target_token != itokens.end(), "token with specified ID does not exist" );
            eosio_assert( target_token->owner == from, "sender does not own token with specified ID");

            const auto& st = *target_token;

            if(st.state == "idle"){
                itokens.modify( st, from, [&]( auto& token ) {
	            token.state = "selling";
                });
            }
            else{
                itokens.modify( st, from, [&]( auto& token ) {
	            token.state = "idle";
                });
            }
        }
    }

    ACTION devtooth_nft::backtogame(name from, string sym, id_type id){
        require_auth(from);

        asset token(0, symbol(symbol_code(sym), 0));

        // UTS
        if( token.symbol == symbol("UTS", 0) ){
            auto target_token = stokens.find( id );
            eosio_assert( target_token != stokens.end(), "token with specified ID does not exist" );
            eosio_assert( target_token->owner == from, "sender does not own token with specified ID");
            eosio_assert( target_token->state == "idle", "Can not back to game in auction");

            const auto& st = *target_token;

            stokens.erase( st ); 
        }
        // UTM
        if( token.symbol == symbol("UTM", 0) ){
            auto target_token = mtokens.find( id );
            eosio_assert( target_token != mtokens.end(), "token with specified ID does not exist" );
            eosio_assert( target_token->owner == from, "sender does not own token with specified ID");
            eosio_assert( target_token->state == "idle", "Can not back to game in auction");

            const auto& st = *target_token;

            mtokens.erase( st ); 
        }
        // UTI
        if( token.symbol == symbol("UTI", 0) ){
            auto target_token = itokens.find( id );
            eosio_assert( target_token != itokens.end(), "token with specified ID does not exist" );
            eosio_assert( target_token->owner == from, "sender does not own token with specified ID");
            eosio_assert( target_token->state == "idle", "Can not back to game in auction");

            const auto& st = *target_token;

            itokens.erase( st ); 
        }
    }

    ACTION devtooth_nft::clean() {
        for(auto iter = stokens.begin(); iter != stokens.end();){
            auto token_iter = stokens.find(iter->primary_key());
            iter++;
            stokens.erase(token_iter);
        }

        for(auto iter2 = mtokens.begin(); iter2 != mtokens.end();){
            auto token_iter2 = mtokens.find(iter2->primary_key());
            iter2++;
            mtokens.erase(token_iter2);
        }

        for(auto iter3 = itokens.begin(); iter3 != itokens.end(); ){
            auto token_iter3 = itokens.find(iter3->primary_key());
            iter3++;
            itokens.erase(token_iter3);
        }
    }

    void devtooth_nft::sub_balance( name owner, asset value ) 
    {
        account_index from_acnts( _self, owner.value );
        const auto& from = from_acnts.get( value.symbol.code().raw(), "no balance object found" );
        eosio_assert( from.balance.amount >= value.amount, "overdrawn balance" );


        if( from.balance.amount == value.amount ) {
            from_acnts.erase( from );
        } else {
            from_acnts.modify( from, owner, [&]( auto& a ) {
                a.balance -= value;
            });
        }
    }

    void devtooth_nft::add_balance( name owner, asset value, name ram_payer )
    {
        account_index to_accounts( _self, owner.value );
        auto to = to_accounts.find( value.symbol.code().raw() );
        if( to == to_accounts.end() ) {
            to_accounts.emplace( ram_payer, [&]( auto& a ){
                a.balance = value;
            });
        } else {
            to_accounts.modify( to, owner, [&]( auto& a ) {
                a.balance += value;
            });
        }
    }

    void devtooth_nft::sub_supply( asset quantity ) {
        auto symbol_name = quantity.symbol.code().raw();
        currency_index currency_table( _self, symbol_name );
        auto current_currency = currency_table.find( symbol_name );

        currency_table.modify( current_currency, _self, [&]( auto& currency ) {
            currency.supply -= quantity;
        });
    }

    void devtooth_nft::add_supply( asset quantity )
    {
        auto symbol_name = quantity.symbol.code().raw();
        currency_index currency_table( _self, symbol_name );
        auto current_currency = currency_table.find( symbol_name );

        currency_table.modify( current_currency, _self, [&]( auto& currency ) {
            currency.supply += quantity;
        });
    }

EOSIO_DISPATCH( devtooth_nft, (create)(issue)(transferid)(changestate)(backtogame)(clean) )

} /// namespace eosio
