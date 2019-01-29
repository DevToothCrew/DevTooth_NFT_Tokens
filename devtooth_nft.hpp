#pragma once

#include <eosiolib/eosio.hpp>
#include <eosiolib/asset.hpp>
#include <string>

namespace eosio {
    using std::string;
    typedef uint64_t id_type;

    class uts : public contract {
    public:
        uts(account_name self) : contract(self), s_tokens(_self, _self), m_tokens(_self, _self), i_tokens(_self, _self) {}

        // @abi action
        void create(account_name issuer, string symbol);

        // @abi action
        void issue(account_name to, asset quantity, uint64_t index);

        // @abi action
        void transferid(account_name from, account_name to, id_type id, string sym);

        // @abi action
        void changestate(account_name from, string sym, id_type id);

        // @abi action 
        void backtogame(account_name from, string sym, id_type id);

        // @abi action
        void clean();

        // servant struct
        struct status_info
        {
            uint32_t basic_str = 0;
            uint32_t basic_dex = 0;
            uint32_t basic_int = 0;
            uint32_t plus_str = 0;
            uint32_t plus_dex = 0;
            uint32_t plus_int = 0;
        };

        struct servant_info
        {
            uint32_t id;
            uint32_t state; //서번트 상태
            uint32_t exp = 0; //서번트 경험치
            uint32_t stat_point = 0;
            status_info status;    //기본 힘,민,지 추가 힘,민,지
            std::vector<uint32_t> equip_slot; //서번트 장비 리스트
        };

        // monster struct
        struct monster_info
        {
            uint32_t id;
            uint32_t state;    //몬스터 상태값
            uint32_t exp = 0;       //경험치
            uint32_t type = 0;     //속성 타입
            uint32_t grade;       // 등급
            uint32_t upgrade = 0; //강화수치
            status_info status;   //기본 힘,민,지 추가 힘,민,지
        };
        
        // item struct
        struct item_info
        {
            uint32_t id;          //아이템 리소스 아이디
            uint32_t state;       //아이템 현재 상태
            uint32_t type;        //장착 타입
            uint32_t tier;        //티어
            uint32_t job;         //직업제한
            uint32_t grade;       //아이템 등급
            uint32_t upgrade = 0; //아이템 강화 수치
            uint32_t atk = 0;
            uint32_t def = 0;
            status_info status; //기본 힘,민,지 추가 힘,민,지
        };

	    // @abi table accounts i64
        struct account {

            asset balance;

            uint64_t primary_key() const { return balance.symbol.name(); }
        };

        // @abi table stat i64
        struct stats {
            asset supply;
            account_name issuer;

            uint64_t primary_key() const { return supply.symbol.name(); }
            account_name get_issuer() const { return issuer; }
        };

        // @abi table utstokens i64
        class utstoken {
            public: 
                id_type idx;          // Unique 64 bit identifier,
                uint32_t t_idx;       // 유저 테이블 상에서의 고유 인덱스
                uint32_t s_idx;       // 서번트 인덱스
                string state;         // 토큰 상태 
    
                account_name owner;  // token owner
                asset value;         // token value (1 UTS)

                id_type primary_key() const { return idx; }
                account_name get_owner() const { return owner; }
        };

         // @abi table utmtokens i64
        class utmtoken {
            public: 
                id_type idx;          // Unique 64 bit identifier,
                uint32_t t_idx;       // 유저 테이블 상에서의 고유 인덱스
                uint32_t m_idx;       // 몬스터 인덱스
                string state;         // 토큰 상태 

                account_name owner;  // token owner
                asset value;         // token value (1 UTM)

                id_type primary_key() const { return idx; }
                account_name get_owner() const { return owner; }
        };

         // @abi table utitokens i64
        class utitoken {
            public: 
                id_type idx;          // Unique 64 bit identifier,
                uint32_t t_idx;       // 유저 테이블 상에서의 고유 인덱스
                uint32_t i_idx;       // 아이템 인덱스
                string state;         // 토큰 상태 

                account_name owner;  // token owner
                asset value;         // token value (1 UTI)

                id_type primary_key() const { return idx; }
                account_name get_owner() const { return owner; }
        };

        // @abi table preservant i64
        struct tservant {
            uint64_t index;
            uint32_t id;
            status_info status;
            
            uint64_t primary_key() const { return index; }
        };

        // @abi table premonster i64
        struct tmonster {
            uint64_t index;
            uint32_t id;
            uint32_t grade;
            status_info status;
            
            uint64_t primary_key() const { return index; }
        };

        // @abi table preitem i64
        struct titem {
            uint64_t index;
            uint32_t id;
            uint32_t type;
            uint32_t tier;
            uint32_t job;
            uint32_t grade;
            uint32_t main_status;
            
            uint64_t primary_key() const { return index; }
        };

	    using account_index = eosio::multi_index<N(accounts), account>;

	    using currency_index = eosio::multi_index<N(stat), stats,
	                       indexed_by< N( byissuer ), const_mem_fun< stats, account_name, &stats::get_issuer> > >;

	    using servant_index = eosio::multi_index<N(utstokens), utstoken,
	                    indexed_by< N( byowner ), const_mem_fun< utstoken, account_name, &utstoken::get_owner> >>;

        using monster_index = eosio::multi_index<N(utmtokens), utmtoken,
	                    indexed_by< N( byowner ), const_mem_fun< utmtoken, account_name, &utmtoken::get_owner> >>;

        using item_index = eosio::multi_index<N(utitokens), utitoken,
	                    indexed_by< N( byowner ), const_mem_fun< utitoken, account_name, &utitoken::get_owner> >>;

        using servant_table = eosio::multi_index<N(preservant), tservant>;
        using monster_table = eosio::multi_index<N(premonster), tmonster>;
        using item_table = eosio::multi_index<N(preitem), titem>;

	    servant_index s_tokens;
        monster_index m_tokens;
        item_index i_tokens;

        void sub_balance(account_name owner, asset value);
        void add_balance(account_name owner, asset value, account_name ram_payer);
        void sub_supply(asset quantity);
        void add_supply(asset quantity);
    };

} /// namespace eosio