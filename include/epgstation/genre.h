/*
 * Copyright (C) 2015-2020 Yuki MIZUNO
 * https://github.com/Harekaze/pvr.epgstation/
 * SPDX-License-Identifier: GPL-3.0-only
 */
#ifndef EPGSTATION_GENRE_H
#define EPGSTATION_GENRE_H
#include "kodi/xbmc_epg_types.h"

// ARIB STD-B10 v5
// https://www.etsi.org/deliver/etsi_en/300400_300499/300468/01.11.01_60/en_300468v011101p.pdf
namespace epgstation {
typedef struct genre_code_t {
    unsigned int main;
    unsigned int sub;
} genre_code;

static genre_code getGenreCodeFromContentNibble(unsigned int level1, unsigned int level2)
{
    switch (level1) {
    case 0x00: { // ニュース/報道
        switch (level2) {
        case 0x00: // 定時・総合
            return { EPG_EVENT_CONTENTMASK_NEWSCURRENTAFFAIRS, 0x00 }; // News/Current Affairs (general)
        case 0x01: // 天気
            return { EPG_EVENT_CONTENTMASK_NEWSCURRENTAFFAIRS, 0x01 }; // News/Weather report
        case 0x02: // 特集・ドキュメント
        case 0x03: // 政治・国会
            return { EPG_EVENT_CONTENTMASK_NEWSCURRENTAFFAIRS, 0x03 }; // Documentary
        case 0x04: // 経済・市況
        case 0x05: // 海外・国際
        case 0x06: // 解説
            return { EPG_EVENT_CONTENTMASK_NEWSCURRENTAFFAIRS, 0x02 }; // News magazine
        case 0x07: // 討論・会談
            return { EPG_EVENT_CONTENTMASK_NEWSCURRENTAFFAIRS, 0x04 }; // Discussion/interview/debate
        case 0x08: // 報道特番
        case 0x09: // ローカル・地域
        case 0x0a: // 交通
        default:
            return { EPG_EVENT_CONTENTMASK_NEWSCURRENTAFFAIRS, 0x00 };
        }
    case 0x01: { // スポーツ
        switch (level2) {
        case 0x00: // スポーツニュース
            return { EPG_EVENT_CONTENTMASK_SPORTS, 0x02 }; // sports magazines
        case 0x01: // 野球
            return { EPG_EVENT_CONTENTMASK_SPORTS, 0x05 }; // team sports (excluding football)
        case 0x02: // サッカー
            return { EPG_EVENT_CONTENTMASK_SPORTS, 0x03 }; // football/scooer
        case 0x03: // ゴルフ
            return { EPG_EVENT_CONTENTMASK_SPORTS, 0x00 }; // sports (general)
        case 0x04: // その他の球技
            return { EPG_EVENT_CONTENTMASK_SPORTS, 0x00 }; // sports (general)
        case 0x05: // 相撲・格闘技
            return { EPG_EVENT_CONTENTMASK_SPORTS, 0x0b }; // martial sports
        case 0x06: // オリンピック・国際大会
            return { EPG_EVENT_CONTENTMASK_SPORTS, 0x01 }; // special events (Olympic Games, World Cup, etc.)
        case 0x07: // マラソン・陸上・水泳
            return { EPG_EVENT_CONTENTMASK_SPORTS, 0x06 }; // athletics
        case 0x08: // モータースポーツ
            return { EPG_EVENT_CONTENTMASK_SPORTS, 0x07 }; // motor sport
        case 0x09: // マリン・ウィンタースポーツ
            return { EPG_EVENT_CONTENTMASK_SPORTS, 0x08 }; // water sport
            // return {EPG_EVENT_CONTENTMASK_SPORTS,0x09}; // winter sport
        case 0x0a: // 競馬・公営競技
            return { EPG_EVENT_CONTENTMASK_SPORTS, 0x0a }; // equestrian
        default:
            return { EPG_EVENT_CONTENTMASK_SPORTS, 0x00 }; // sports (general)
        }
    }
    case 0x02: { // 情報/ワイドショー
        switch (level2) {
        case 0x00: // 芸能・ワイドショー
            return { EPG_EVENT_CONTENTMASK_ARTSCULTURE, 0x04 }; // popular culture/traditional arts
        case 0x01: // ファッション
            return { EPG_EVENT_CONTENTMASK_ARTSCULTURE, 0x0b }; // fashion
        case 0x02: // 暮らし・住まい
            return { EPG_EVENT_CONTENTMASK_ARTSCULTURE, 0x00 }; // arts/culture (without music, general)
        case 0x03: // 健康・医療
            return { EPG_EVENT_CONTENTMASK_LEISUREHOBBIES, 0x04 }; // fitness and health
        case 0x04: // ショッピング・通販
            return { EPG_EVENT_CONTENTMASK_LEISUREHOBBIES, 0x06 }; // advertisement/shopping
        case 0x05: // グルメ・料理
            return { EPG_EVENT_CONTENTMASK_LEISUREHOBBIES, 0x05 }; // cooking
        case 0x06: // イベント
        case 0x07: // 番組紹介・お知らせ
            return { EPG_EVENT_CONTENTMASK_ARTSCULTURE, 0x08 }; // broadcasting/press
        default:
            return { EPG_EVENT_CONTENTMASK_ARTSCULTURE, 0x00 }; // arts/culture (without music, general)
        }
    }
    case 0x03: { // ドラマ
        switch (level2) {
        case 0x00: // 国内ドラマ
        case 0x01: // 海外ドラマ
            return { EPG_EVENT_CONTENTMASK_MOVIEDRAMA, 0x00 }; // movie/drama (general)
        case 0x02: // 時代劇
            return { EPG_EVENT_CONTENTMASK_MOVIEDRAMA, 0x07 }; // serious/classical/religious/historical movie/drama
        default:
            return { EPG_EVENT_CONTENTMASK_MOVIEDRAMA, 0x00 }; // movie/drama (general)
        }
    }

    case 0x04: { // 音楽
        switch (level2) {
        case 0x00: // 国内ロック・ポップス
        case 0x01: // 海外ロック・ポップス
            return { EPG_EVENT_CONTENTMASK_MUSICBALLETDANCE, 0x01 }; // rock/pop
        case 0x02: // クラシック・オペラ
            return { EPG_EVENT_CONTENTMASK_MUSICBALLETDANCE, 0x02 }; // serious music/classical music
        case 0x03: // ジャズ・フュージョン
            return { EPG_EVENT_CONTENTMASK_MUSICBALLETDANCE, 0x04 }; // jazz
        case 0x04: // 歌謡曲・演歌
            return { EPG_EVENT_CONTENTMASK_MUSICBALLETDANCE, 0x03 }; // folk/traditional music
        case 0x05: // ライブ・コンサート
            return { EPG_EVENT_CONTENTMASK_MUSICBALLETDANCE, 0x05 }; // musical/opera
        case 0x06: // ランキング・リクエスト
        case 0x07: // カラオケ・のど自慢
            return { EPG_EVENT_CONTENTMASK_MUSICBALLETDANCE, 0x00 }; // music/ballet/dance (general)
        case 0x08: // 民謡・邦楽
        case 0x09: // 童謡・キッズ
        case 0x0a: // 民族音楽・ワールドミュージック
            return { EPG_EVENT_CONTENTMASK_MUSICBALLETDANCE, 0x03 }; // folk/traditional music
        default:
            return { EPG_EVENT_CONTENTMASK_MUSICBALLETDANCE, 0x00 }; // music/ballet/dance (general)
        }
    }
    case 0x05: { // バラエティ
        switch (level2) {
        case 0x00: // クイズ
        case 0x01: // ゲーム
            return { EPG_EVENT_CONTENTMASK_SHOW, 0x01 }; // game show/quiz/contest
        case 0x02: // トークバラエティ
            return { EPG_EVENT_CONTENTMASK_SHOW, 0x03 }; // talk show
        case 0x03: // お笑い・コメディ
            return { EPG_EVENT_CONTENTMASK_SHOW, 0x02 }; // variety show
        case 0x04: // 音楽バラエティ
        case 0x05: // 旅バラエティ
        case 0x06: // 料理バラエティ
        default:
            return { EPG_EVENT_CONTENTMASK_SHOW, 0x00 }; // show/game show (general)
        }
    }
    case 0x06: { // 映画
        switch (level2) {
        case 0x00: // 洋画
        case 0x01: // 邦画
        case 0x02: // アニメ
        default:
            return { EPG_EVENT_CONTENTMASK_MOVIEDRAMA, 0x00 }; // movie/drama (general)
        }
    }
    case 0x07: { // アニメ/特撮
        switch (level2) {
        case 0x00: // 国内アニメ
        case 0x01: // 海外アニメ
        case 0x02: // 特撮
        default:
            return { EPG_EVENT_CONTENTMASK_CHILDRENYOUTH, 0x05 }; // cartoons/puppets
        }
    }
    case 0x08: { // ドキュメンタリー/教養
        switch (level2) {
        case 0x00: // 社会・時事
            return { EPG_EVENT_CONTENTMASK_EDUCATIONALSCIENCE, 0x05 }; // social/spiritual sciences
        case 0x01: // 歴史・紀行
            return { EPG_EVENT_CONTENTMASK_EDUCATIONALSCIENCE, 0x04 }; // foreign countries/expeditions
        case 0x02: // 自然・動物・環境
            return { EPG_EVENT_CONTENTMASK_EDUCATIONALSCIENCE, 0x01 }; // nature/animals/environment
        case 0x03: // 宇宙・科学・医学
            return { EPG_EVENT_CONTENTMASK_EDUCATIONALSCIENCE, 0x03 }; // medicine/physiology/psychology
        case 0x04: // カルチャー・伝統文化
        case 0x05: // 文学・文芸
        case 0x06: // スポーツ
        case 0x07: // ドキュメンタリー全般
        case 0x08: // インタビュー・討論
        default:
            return { EPG_EVENT_CONTENTMASK_EDUCATIONALSCIENCE, 0x00 }; // education/science/factual topics (general)
        }
    }
    case 0x09: { // 劇場/公演
        switch (level2) {
        case 0x00: // 現代劇・新劇
            return { EPG_EVENT_CONTENTMASK_MUSICBALLETDANCE, 0x03 }; // folk/traditional music
        case 0x01: // ミュージカル
            return { EPG_EVENT_CONTENTMASK_MUSICBALLETDANCE, 0x05 }; // musical/opera
        case 0x02: // ダンス・バレエ
            return { EPG_EVENT_CONTENTMASK_MUSICBALLETDANCE, 0x06 }; // ballet
        case 0x03: // 落語・演芸
        case 0x04: // 歌舞伎・古典
            return { EPG_EVENT_CONTENTMASK_MUSICBALLETDANCE, 0x03 }; // folk/traditional music
        default:
            return { EPG_EVENT_CONTENTMASK_MUSICBALLETDANCE, 0x00 }; // music/ballet/dance (general)
        }
    }
    case 0x0a: { // 趣味/教育
        switch (level2) {
        case 0x00: // 旅・釣り・アウトドア
            return { EPG_EVENT_CONTENTMASK_LEISUREHOBBIES, 0x01 }; // tourism/travel
        case 0x01: // 園芸・ペット・手芸
            return { EPG_EVENT_CONTENTMASK_LEISUREHOBBIES, 0x07 }; // gardening
            // return {EPG_EVENT_CONTENTMASK_LEISUREHOBBIES,0x02}; // handicraft
        case 0x02: // 音楽・美術・工芸
        case 0x03: // 囲碁・将棋
        case 0x04: // 麻雀・パチンコ
            return { EPG_EVENT_CONTENTMASK_LEISUREHOBBIES, 0x00 }; // leisure hobbies (general)
        case 0x05: // 車・オートバイ
            return { EPG_EVENT_CONTENTMASK_LEISUREHOBBIES, 0x03 }; // motoring
        case 0x06: // コンピュータ・TVゲーム
            return { EPG_EVENT_CONTENTMASK_EDUCATIONALSCIENCE, 0x02 }; // technology/natural sciences
        case 0x07: // 会話・語学
            return { EPG_EVENT_CONTENTMASK_EDUCATIONALSCIENCE, 0x07 }; // languages
        case 0x08: // 幼児・小学生
        case 0x09: // 中学生・高校生
        case 0x0a: // 大学生・受験
            return { EPG_EVENT_CONTENTMASK_EDUCATIONALSCIENCE, 0x00 }; // education/science/factual topics (general)
        case 0x0b: // 生涯教育・資格
            return { EPG_EVENT_CONTENTMASK_EDUCATIONALSCIENCE, 0x06 }; // further education
        case 0x0c: // 教育問題
        default:
            return { EPG_EVENT_CONTENTMASK_EDUCATIONALSCIENCE, 0x00 }; // education/science/factual topics (general)
        }
    }
    case 0x0b: { // 福祉
        switch (level2) {
        case 0x00: // 高齢者
        case 0x01: // 障害者
        case 0x02: // 社会福祉
        case 0x03: // ボランティア
        case 0x04: // 手話
        case 0x05: // 文字(字幕)
        case 0x06: // 音声解説
        default:
            return { EPG_EVENT_CONTENTMASK_EDUCATIONALSCIENCE, 0x00 }; // education/science/factual topics (general)
        }
    }
    default:
        return { EPG_EVENT_CONTENTMASK_UNDEFINED, 0x00 };
    }
    }
}
} // namespace epgstation

#endif /* end of include guard */
