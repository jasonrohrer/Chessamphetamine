#ifndef PIECE_SPECS_H_INCLUDED
#define PIECE_SPECS_H_INCLUDED


#define PIECE_VALUE_LIST( C, V ) \
    V( C,   0,  noPiece,        0                       ) \
    V( C,   1,  pawn,           1000                    ) \
    V( C,   2,  bishop,         3200                    ) \
    V( C,   3,  knight,         3000                    ) \
    V( C,   4,  rook,           5000                    ) \
    V( C,   5,  queen,          9000                    ) \
    V( C,   6,  king,           999900                  ) \
    V( C,   7,  laserRook,      7000                    ) \
    V( C,   8,  laserPawn,      2000                    ) \
    V( C,   9,  doublingPawn,   2000                    ) \
    V( C,  10,  addingRook,     6000                    ) \
    V( C,  11,  rocket,         2000                    ) \
    V( C,  12,  laserBishop,    4000                    ) \
    V( C,  13,  laserQueen,     11000                   ) \
    V( C,  14,  doublingQueen,  10000                   ) \
    V( C,  15,  rublePawn,      1000                    ) \


#define PIECE_SYMB_LIST( C, V ) \
    V( C,   0,  noPiece,        "  "                    ) \
    V( C,   1,  pawn,           "pp"                    ) \
    V( C,   2,  bishop,         "bb"                    ) \
    V( C,   3,  knight,         "nn"                    ) \
    V( C,   4,  rook,           "rr"                    ) \
    V( C,   5,  queen,          "qq"                    ) \
    V( C,   6,  king,           "kk"                    ) \
    V( C,   7,  laserRook,      "lr"                    ) \
    V( C,   8,  laserPawn,      "lp"                    ) \
    V( C,   9,  doublingPawn,   "dp"                    ) \
    V( C,  10,  addingRook,     "ar"                    ) \
    V( C,  11,  rocket,         "vv"                    ) \
    V( C,  12,  laserBishop,    "lb"                    ) \
    V( C,  13,  laserQueen,     "lq"                    ) \
    V( C,  14,  doublingQueen,  "dq"                    ) \
    V( C,  15,  rublePawn,      "rp"                    ) \


#define PIECE_NAME_LIST( C, V ) \
    V( C,   0,  noPiece,        "noPiece"               ) \
    V( C,   1,  pawn,           "pawn"                  ) \
    V( C,   2,  bishop,         "bishop"                ) \
    V( C,   3,  knight,         "knight"                ) \
    V( C,   4,  rook,           "rook"                  ) \
    V( C,   5,  queen,          "queen"                 ) \
    V( C,   6,  king,           "king"                  ) \
    V( C,   7,  laserRook,      "laserRook"             ) \
    V( C,   8,  laserPawn,      "laserPawn"             ) \
    V( C,   9,  doublingPawn,   "doublingPawn"          ) \
    V( C,  10,  addingRook,     "addingRook"            ) \
    V( C,  11,  rocket,         "rocket"                ) \
    V( C,  12,  laserBishop,    "laserBishop"           ) \
    V( C,  13,  laserQueen,     "laserQueen"            ) \
    V( C,  14,  doublingQueen,  "doublingQueen"         ) \
    V( C,  15,  rublePawn,      "rublePawn"             ) \


#define MOVE_FUNCTION_LIST( C, V ) \
    V( C,   0,  noPiece,        noPieceMove             ) \
    V( C,   1,  pawn,           pawnMove                ) \
    V( C,   2,  bishop,         bishopMove              ) \
    V( C,   3,  knight,         knightMove              ) \
    V( C,   4,  rook,           rookMove                ) \
    V( C,   5,  queen,          queenMove               ) \
    V( C,   6,  king,           kingMove                ) \
    V( C,   7,  laserRook,      laserRookMove           ) \
    V( C,   8,  laserPawn,      laserPawnMove           ) \
    V( C,   9,  doublingPawn,   doublingPawnMove        ) \
    V( C,  10,  addingRook,     rookMove                ) \
    V( C,  11,  rocket,         rocketMove              ) \
    V( C,  12,  laserBishop,    laserBishopMove         ) \
    V( C,  13,  laserQueen,     laserQueenMove          ) \
    V( C,  14,  doublingQueen,  queenMove               ) \
    V( C,  15,  rublePawn,      pawnMove                ) \


#define REPICK_MOVE_AT_END_LIST( C, V ) \
    V( C,   0,  noPiece,        0                       ) \
    V( C,   1,  pawn,           0                       ) \
    V( C,   2,  bishop,         0                       ) \
    V( C,   3,  knight,         0                       ) \
    V( C,   4,  rook,           0                       ) \
    V( C,   5,  queen,          0                       ) \
    V( C,   6,  king,           0                       ) \
    V( C,   7,  laserRook,      0                       ) \
    V( C,   8,  laserPawn,      0                       ) \
    V( C,   9,  doublingPawn,   0                       ) \
    V( C,  10,  addingRook,     0                       ) \
    V( C,  11,  rocket,         1                       ) \
    V( C,  12,  laserBishop,    0                       ) \
    V( C,  13,  laserQueen,     0                       ) \
    V( C,  14,  doublingQueen,  0                       ) \
    V( C,  15,  rublePawn,      0                       ) \


#define EFFECTS_FUNCTION_LIST( C, V ) \
    V( C,   0,  noPiece,        nullEffects             ) \
    V( C,   1,  pawn,           nullEffects             ) \
    V( C,   2,  bishop,         nullEffects             ) \
    V( C,   3,  knight,         nullEffects             ) \
    V( C,   4,  rook,           nullEffects             ) \
    V( C,   5,  queen,          nullEffects             ) \
    V( C,   6,  king,           nullEffects             ) \
    V( C,   7,  laserRook,      nullEffects             ) \
    V( C,   8,  laserPawn,      nullEffects             ) \
    V( C,   9,  doublingPawn,   doublingPawnEffects     ) \
    V( C,  10,  addingRook,     addingRookEffects       ) \
    V( C,  11,  rocket,         nullEffects             ) \
    V( C,  12,  laserBishop,    nullEffects             ) \
    V( C,  13,  laserQueen,     nullEffects             ) \
    V( C,  14,  doublingQueen,  doublingQueenEffects    ) \
    V( C,  15,  rublePawn,      nullEffects             ) \


#define PIECE_CAPTURE_MONEY_LIST( C, V ) \
    V( C,   0,  noPiece,        0                       ) \
    V( C,   1,  pawn,           1                       ) \
    V( C,   2,  bishop,         NON_PAWN_CAPTURE_VALUE  ) \
    V( C,   3,  knight,         NON_PAWN_CAPTURE_VALUE  ) \
    V( C,   4,  rook,           NON_PAWN_CAPTURE_VALUE  ) \
    V( C,   5,  queen,          NON_PAWN_CAPTURE_VALUE  ) \
    V( C,   6,  king,           6                       ) \
    V( C,   7,  laserRook,      NON_PAWN_CAPTURE_VALUE  ) \
    V( C,   8,  laserPawn,      NON_PAWN_CAPTURE_VALUE  ) \
    V( C,   9,  doublingPawn,   NON_PAWN_CAPTURE_VALUE  ) \
    V( C,  10,  addingRook,     NON_PAWN_CAPTURE_VALUE  ) \
    V( C,  11,  rocket,         NON_PAWN_CAPTURE_VALUE  ) \
    V( C,  12,  laserBishop,    NON_PAWN_CAPTURE_VALUE  ) \
    V( C,  13,  laserQueen,     NON_PAWN_CAPTURE_VALUE  ) \
    V( C,  14,  doublingQueen,  NON_PAWN_CAPTURE_VALUE  ) \
    V( C,  15,  rublePawn,      1                       ) \


#define INIT_FUNCTION_LIST( C, V ) \
    V( C,   0,  noPiece,        noPieceInit             ) \
    V( C,   1,  pawn,           defaultPieceInit        ) \
    V( C,   2,  bishop,         defaultPieceInit        ) \
    V( C,   3,  knight,         defaultPieceInit        ) \
    V( C,   4,  rook,           defaultPieceInit        ) \
    V( C,   5,  queen,          defaultPieceInit        ) \
    V( C,   6,  king,           defaultPieceInit        ) \
    V( C,   7,  laserRook,      laserPieceInit          ) \
    V( C,   8,  laserPawn,      laserPieceInit          ) \
    V( C,   9,  doublingPawn,   defaultPieceInit        ) \
    V( C,  10,  addingRook,     defaultPieceInit        ) \
    V( C,  11,  rocket,         rocketPieceInit         ) \
    V( C,  12,  laserBishop,    laserPieceInit          ) \
    V( C,  13,  laserQueen,     laserPieceInit          ) \
    V( C,  14,  doublingQueen,  defaultPieceInit        ) \
    V( C,  15,  rublePawn,      moneyPieceInit          ) \


#define STEP_FUNCTION_LIST( C, V ) \
    V( C,   0,  noPiece,        noPieceStep             ) \
    V( C,   1,  pawn,           defaultPieceStep        ) \
    V( C,   2,  bishop,         defaultPieceStep        ) \
    V( C,   3,  knight,         defaultPieceStep        ) \
    V( C,   4,  rook,           defaultPieceStep        ) \
    V( C,   5,  queen,          defaultPieceStep        ) \
    V( C,   6,  king,           defaultPieceStep        ) \
    V( C,   7,  laserRook,      multiPhaseStep          ) \
    V( C,   8,  laserPawn,      multiPhaseStep          ) \
    V( C,   9,  doublingPawn,   defaultPieceStep        ) \
    V( C,  10,  addingRook,     defaultPieceStep        ) \
    V( C,  11,  rocket,         multiPhaseStep          ) \
    V( C,  12,  laserBishop,    multiPhaseStep          ) \
    V( C,  13,  laserQueen,     multiPhaseStep          ) \
    V( C,  14,  doublingQueen,  defaultPieceStep        ) \
    V( C,  15,  rublePawn,      multiPhaseStep          ) \


#define DRAW_FUNCTION_LIST( C, V ) \
    V( C,   0,  noPiece,        noPieceDraw             ) \
    V( C,   1,  pawn,           defaultPieceDraw        ) \
    V( C,   2,  bishop,         defaultPieceDraw        ) \
    V( C,   3,  knight,         defaultPieceDraw        ) \
    V( C,   4,  rook,           defaultPieceDraw        ) \
    V( C,   5,  queen,          defaultPieceDraw        ) \
    V( C,   6,  king,           defaultPieceDraw        ) \
    V( C,   7,  laserRook,      multiPhaseDraw          ) \
    V( C,   8,  laserPawn,      multiPhaseDraw          ) \
    V( C,   9,  doublingPawn,   defaultPieceDraw        ) \
    V( C,  10,  addingRook,     defaultPieceDraw        ) \
    V( C,  11,  rocket,         multiPhaseDraw          ) \
    V( C,  12,  laserBishop,    multiPhaseDraw          ) \
    V( C,  13,  laserQueen,     multiPhaseDraw          ) \
    V( C,  14,  doublingQueen,  defaultPieceDraw        ) \
    V( C,  15,  rublePawn,      multiPhaseDraw          ) \


#define PIECE_NAME_KEY_STRING_LIST( C, V ) \
    V( C,   0,  noPiece,        ""                      ) \
    V( C,   1,  pawn,           "pawn"                  ) \
    V( C,   2,  bishop,         "bishop"                ) \
    V( C,   3,  knight,         "knight"                ) \
    V( C,   4,  rook,           "rook"                  ) \
    V( C,   5,  queen,          "queen"                 ) \
    V( C,   6,  king,           "king"                  ) \
    V( C,   7,  laserRook,      "laserRook"             ) \
    V( C,   8,  laserPawn,      "laserPawn"             ) \
    V( C,   9,  doublingPawn,   "doublingPawn"          ) \
    V( C,  10,  addingRook,     "addingRook"            ) \
    V( C,  11,  rocket,         "rocket"                ) \
    V( C,  12,  laserBishop,    "laserBishop"           ) \
    V( C,  13,  laserQueen,     "laserQueen"            ) \
    V( C,  14,  doublingQueen,  "doublingQueen"         ) \
    V( C,  15,  rublePawn,      "rublePawn"             ) \


#define PIECE_DESCRIPTION_KEY_STRING_LIST( C, V ) \
    V( C,   0,  noPiece,        ""                      ) \
    V( C,   1,  pawn,           "pawnDesc"              ) \
    V( C,   2,  bishop,         "bishopDesc"            ) \
    V( C,   3,  knight,         "knightDesc"            ) \
    V( C,   4,  rook,           "rookDesc"              ) \
    V( C,   5,  queen,          "queenDesc"             ) \
    V( C,   6,  king,           ""                      ) \
    V( C,   7,  laserRook,      "laserRookDesc"         ) \
    V( C,   8,  laserPawn,      "laserPawnDesc"         ) \
    V( C,   9,  doublingPawn,   "doublingPawnDesc"      ) \
    V( C,  10,  addingRook,     "addingRookDesc"        ) \
    V( C,  11,  rocket,         "rocketDesc"            ) \
    V( C,  12,  laserBishop,    "laserBishopDesc"       ) \
    V( C,  13,  laserQueen,     "laserQueenDesc"        ) \
    V( C,  14,  doublingQueen,  "doublingQueenDesc"     ) \
    V( C,  15,  rublePawn,      "rublePawnDesc"         ) \


#define PIECE_SPRITE_FILE_LIST( C, V ) \
    V( C,   0,  noPiece,        ""                      ) \
    V( C,   1,  pawn,           "pawn.tga"              ) \
    V( C,   2,  bishop,         "bishop.tga"            ) \
    V( C,   3,  knight,         "knight.tga"            ) \
    V( C,   4,  rook,           "rook.tga"              ) \
    V( C,   5,  queen,          "queen.tga"             ) \
    V( C,   6,  king,           "king.tga"              ) \
    V( C,   7,  laserRook,      "rook.tga"              ) \
    V( C,   8,  laserPawn,      "pawn.tga"              ) \
    V( C,   9,  doublingPawn,   "doublingPawn.tga"      ) \
    V( C,  10,  addingRook,     "addingRook.tga"        ) \
    V( C,  11,  rocket,         "rocket.tga"            ) \
    V( C,  12,  laserBishop,    "bishop.tga"            ) \
    V( C,  13,  laserQueen,     "queen.tga"             ) \
    V( C,  14,  doublingQueen,  "doublingQueen.tga"     ) \
    V( C,  15,  rublePawn,      "rublePawn.tga"         ) \


#define PIECE_EXTRA_OFFSET_Y_LIST( C, V ) \
    V( C,   0,  noPiece,        ARRAY_PAIR(  0,  0 )    ) \
    V( C,   1,  pawn,           ARRAY_PAIR(  0,  0 )    ) \
    V( C,   2,  bishop,         ARRAY_PAIR(  0,  0 )    ) \
    V( C,   3,  knight,         ARRAY_PAIR(  0,  0 )    ) \
    V( C,   4,  rook,           ARRAY_PAIR(  0,  0 )    ) \
    V( C,   5,  queen,          ARRAY_PAIR(  0,  0 )    ) \
    V( C,   6,  king,           ARRAY_PAIR(  0,  0 )    ) \
    V( C,   7,  laserRook,      ARRAY_PAIR(  2,  2 )    ) \
    V( C,   8,  laserPawn,      ARRAY_PAIR( -1,  0 )    ) \
    V( C,   9,  doublingPawn,   ARRAY_PAIR(  0,  0 )    ) \
    V( C,  10,  addingRook,     ARRAY_PAIR(  0,  0 )    ) \
    V( C,  11,  rocket,         ARRAY_PAIR(  0,  0 )    ) \
    V( C,  12,  laserBishop,    ARRAY_PAIR(  0,  0 )    ) \
    V( C,  13,  laserQueen,     ARRAY_PAIR(  6,  6 )    ) \
    V( C,  14,  doublingQueen,  ARRAY_PAIR(  0,  0 )    ) \
    V( C,  15,  rublePawn,      ARRAY_PAIR(  0,  0 )    ) \


#define PIECE_EXTRA_SPRITE_LIST( C, V ) \
    V( C,   0,  noPiece,        ARRAY_PAIR( "", "" )    ) \
    V( C,   1,  pawn,           ARRAY_PAIR( "", "" )    ) \
    V( C,   2,  bishop,         ARRAY_PAIR( "", "" )    ) \
    V( C,   3,  knight,         ARRAY_PAIR( "", "" )    ) \
    V( C,   4,  rook,           ARRAY_PAIR( "", "" )    ) \
    V( C,   5,  queen,          ARRAY_PAIR( "", "" )    ) \
    V( C,   6,  king,           ARRAY_PAIR( "", "" )    ) \
    V( C,   7,  laserRook,      ARRAY_PAIR( "rookLasers.tga", "rookLasers.tga" ) ) \
    V( C,   8,  laserPawn,      ARRAY_PAIR( "pawnLasersWhite.tga", "pawnLasersBlack.tga" ) ) \
    V( C,   9,  doublingPawn,   ARRAY_PAIR( "", "" )    ) \
    V( C,  10,  addingRook,     ARRAY_PAIR( "", "" )    ) \
    V( C,  11,  rocket,         ARRAY_PAIR( "", "" )    ) \
    V( C,  12,  laserBishop,    ARRAY_PAIR( "bishopLasers.tga", "bishopLasers.tga" ) ) \
    V( C,  13,  laserQueen,     ARRAY_PAIR( "queenLasers.tga", "queenLasers.tga" ) ) \
    V( C,  14,  doublingQueen,  ARRAY_PAIR( "", "" )    ) \
    V( C,  15,  rublePawn,      ARRAY_PAIR( "", "" )    ) \


#define SPECIAL_RETURN_FUNCTION_LIST( C, V ) \
    V( C,   0,  noPiece,        noSpecialReturn         ) \
    V( C,   1,  pawn,           noSpecialReturn         ) \
    V( C,   2,  bishop,         noSpecialReturn         ) \
    V( C,   3,  knight,         noSpecialReturn         ) \
    V( C,   4,  rook,           noSpecialReturn         ) \
    V( C,   5,  queen,          noSpecialReturn         ) \
    V( C,   6,  king,           noSpecialReturn         ) \
    V( C,   7,  laserRook,      noSpecialReturn         ) \
    V( C,   8,  laserPawn,      noSpecialReturn         ) \
    V( C,   9,  doublingPawn,   noSpecialReturn         ) \
    V( C,  10,  addingRook,     noSpecialReturn         ) \
    V( C,  11,  rocket,         rocketSpecialReturn     ) \
    V( C,  12,  laserBishop,    noSpecialReturn         ) \
    V( C,  13,  laserQueen,     noSpecialReturn         ) \
    V( C,  14,  doublingQueen,  noSpecialReturn         ) \
    V( C,  15,  rublePawn,      noSpecialReturn         ) \


#define PIECE_RARITY_LIST( C, V ) \
    V( C,   0,  noPiece,        contraband              ) \
    V( C,   1,  pawn,           rare                    ) \
    V( C,   2,  bishop,         common                  ) \
    V( C,   3,  knight,         common                  ) \
    V( C,   4,  rook,           uncommon                ) \
    V( C,   5,  queen,          contraband              ) \
    V( C,   6,  king,           contraband              ) \
    V( C,   7,  laserRook,      legendary               ) \
    V( C,   8,  laserPawn,      rare                    ) \
    V( C,   9,  doublingPawn,   rare                    ) \
    V( C,  10,  addingRook,     legendary               ) \
    V( C,  11,  rocket,         rare                    ) \
    V( C,  12,  laserBishop,    rare                    ) \
    V( C,  13,  laserQueen,     contraband              ) \
    V( C,  14,  doublingQueen,  contraband              ) \
    V( C,  15,  rublePawn,      rare                    ) \


#define SHOP_PRICE_LIST( C, V ) \
    V( C,   0,  noPiece,        0                       ) \
    V( C,   1,  pawn,           1                       ) \
    V( C,   2,  bishop,         1                       ) \
    V( C,   3,  knight,         3                       ) \
    V( C,   4,  rook,           5                       ) \
    V( C,   5,  queen,          0                       ) \
    V( C,   6,  king,           0                       ) \
    V( C,   7,  laserRook,      30                      ) \
    V( C,   8,  laserPawn,      8                       ) \
    V( C,   9,  doublingPawn,   8                       ) \
    V( C,  10,  addingRook,     14                      ) \
    V( C,  11,  rocket,         8                       ) \
    V( C,  12,  laserBishop,    20                      ) \
    V( C,  13,  laserQueen,     0                       ) \
    V( C,  14,  doublingQueen,  0                       ) \
    V( C,  15,  rublePawn,      1                       ) \


#define PIECE_DEPLOYMENT_COST_LIST( C, V ) \
    V( C,   0,  noPiece,        0                       ) \
    V( C,   1,  pawn,           2                       ) \
    V( C,   2,  bishop,         2                       ) \
    V( C,   3,  knight,         1                       ) \
    V( C,   4,  rook,           5                       ) \
    V( C,   5,  queen,          6                       ) \
    V( C,   6,  king,           0                       ) \
    V( C,   7,  laserRook,      6                       ) \
    V( C,   8,  laserPawn,      3                       ) \
    V( C,   9,  doublingPawn,   2                       ) \
    V( C,  10,  addingRook,     6                       ) \
    V( C,  11,  rocket,         2                       ) \
    V( C,  12,  laserBishop,    5                       ) \
    V( C,  13,  laserQueen,     8                       ) \
    V( C,  14,  doublingQueen,  7                       ) \
    V( C,  15,  rublePawn,      2                       ) \


#define POST_EFFECTS_FUNCTION_LIST( C, V ) \
    V( C,   0,  noPiece,        nullPostEffects         ) \
    V( C,   1,  pawn,           nullPostEffects         ) \
    V( C,   2,  bishop,         nullPostEffects         ) \
    V( C,   3,  knight,         nullPostEffects         ) \
    V( C,   4,  rook,           nullPostEffects         ) \
    V( C,   5,  queen,          nullPostEffects         ) \
    V( C,   6,  king,           nullPostEffects         ) \
    V( C,   7,  laserRook,      nullPostEffects         ) \
    V( C,   8,  laserPawn,      nullPostEffects         ) \
    V( C,   9,  doublingPawn,   nullPostEffects         ) \
    V( C,  10,  addingRook,     nullPostEffects         ) \
    V( C,  11,  rocket,         nullPostEffects         ) \
    V( C,  12,  laserBishop,    nullPostEffects         ) \
    V( C,  13,  laserQueen,     nullPostEffects         ) \
    V( C,  14,  doublingQueen,  nullPostEffects         ) \
    V( C,  15,  rublePawn,      oneRubleEffects         ) \


#endif
