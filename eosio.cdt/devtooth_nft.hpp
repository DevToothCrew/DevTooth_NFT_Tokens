#pragma once

#include <eosiolib/eosio.hpp>
#include <eosiolib/asset.hpp>
#include <string>

namespace eosio
{
using std::string;
typedef uint64_t id_type;

CONTRACT devtooth_nft : public contract
{
  public:
    devtooth_nft(name _self, name _code, datastream<const char *> ds) : contract(_self, _code, ds), stokens(_self, _self.value), mtokens(_self, _self.value), itokens(_self, _self.value) {}

    ACTION create(name issuer, string symbol);

    ACTION issue(name to, asset quantity, uint64_t index);

    ACTION transferid(name from, name to, id_type id, string sym);

    ACTION changestate(name from, string sym, id_type id);

    ACTION backtogame(name from, string sym, id_type id);

    ACTION clean();

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
        uint32_t state;   //서번트 상태
        uint32_t exp = 0; //서번트 경험치
        uint32_t stat_point = 0;
        status_info status;               //기본 힘,민,지 추가 힘,민,지
        std::vector<uint32_t> equip_slot; //서번트 장비 리스트
    };

    // monster struct
    struct monster_info
    {
        uint32_t id;
        uint32_t state;       //몬스터 상태값
        uint32_t exp = 0;     //경험치
        uint32_t type = 0;    //속성 타입
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

    TABLE account
    {

        asset balance;

        uint64_t primary_key() const { return balance.symbol.code().raw(); }
    };

    TABLE stats
    {
        asset supply;
        name issuer;

        uint64_t primary_key() const { return supply.symbol.code().raw(); }
        uint64_t get_issuer() const { return issuer.value; }
    };

    TABLE utstoken
    {
        id_type idx;    // Unique 64 bit identifier,
        uint32_t t_idx; // 유저 테이블 상에서의 고유 인덱스
        string state;   // 토큰 상태

        name owner;  // token owner
        name master; // token master for search detail info
        asset value; // token value (1 UTS)

        id_type primary_key() const { return idx; }
        uint64_t get_owner() const { return owner.value; }
    };

    TABLE utmtoken
    {
        id_type idx;    // Unique 64 bit identifier,
        uint32_t t_idx; // 유저 테이블 상에서의 고유 인덱스
        string state;   // 토큰 상태

        name owner;  // token owner
        name master; // token master for search detail info
        asset value; // token value (1 UTM)

        id_type primary_key() const { return idx; }
        uint64_t get_owner() const { return owner.value; }
    };

    TABLE utitoken
    {
        id_type idx;    // Unique 64 bit identifier,
        uint32_t t_idx; // 유저 테이블 상에서의 고유 인덱스
        string state;   // 토큰 상태

        name owner;  // token owner
        name master; // token master for search detail info
        asset value; // token value (1 UTI)

        id_type primary_key() const { return idx; }
        uint64_t get_owner() const { return owner.value; }
    };

    TABLE tservant
    {
        uint64_t index;
        uint32_t id;
        status_info status;

        uint64_t primary_key() const { return index; }
    };

    TABLE tmonster
    {
        uint64_t index;
        uint32_t id;
        uint32_t grade;
        status_info status;

        uint64_t primary_key() const { return index; }
    };

    TABLE titem
    {
        uint64_t index;
        uint32_t id;
        uint32_t type;
        uint32_t tier;
        uint32_t job;
        uint32_t grade;
        uint32_t main_status;

        uint64_t primary_key() const { return index; }
    };

    typedef eosio::multi_index<"accounts"_n, account> account_index;

    typedef eosio::multi_index<"stat"_n, stats, indexed_by<"byissuer"_n, const_mem_fun<stats, uint64_t, &stats::get_issuer>>> currency_index;

    typedef eosio::multi_index<"utstokens"_n, utstoken, indexed_by<"byowner"_n, const_mem_fun<utstoken, uint64_t, &utstoken::get_owner>>> servant_index;

    typedef eosio::multi_index<"utmtokens"_n, utmtoken, indexed_by<"byowner"_n, const_mem_fun<utmtoken, uint64_t, &utmtoken::get_owner>>> monster_index;

    typedef eosio::multi_index<"utitokens"_n, utitoken, indexed_by<"byowner"_n, const_mem_fun<utitoken, uint64_t, &utitoken::get_owner>>> item_index;

    using servant_table = eosio::multi_index<"preservant"_n, tservant>;
    using monster_table = eosio::multi_index<"premonster"_n, tmonster>;
    using item_table = eosio::multi_index<"preitem"_n, titem>;

    servant_index stokens;
    monster_index mtokens;
    item_index itokens;

    void sub_balance(name owner, asset value);
    void add_balance(name owner, asset value, name ram_payer);
    void sub_supply(asset quantity);
    void add_supply(asset quantity);
};

} // namespace eosio