static void CreateBanner(
	struct game_state *state,
	struct screen_buffer *buffer)
{
	float xAnchor = (float)buffer->x * 0.25f;
	float yAnchor = (float)buffer->y - 80.0f;	
	
	union vec2 bannerPos = {
		.x = xAnchor - 10.0f,
		.y = yAnchor
	};
	union vec2 bannerDim = {
		.x = (float)buffer->x - 10 - bannerPos.x,
		.y = 30.0f
	};
	struct entity *bannerRect = NewEntity(state, bannerPos, bannerDim, ENTTYPE_GENERICRECT);
	bannerRect->rect.colour = COL_GREEN;
	
	union vec2 bannerDecorPos = {
		.x = bannerPos.x + 3.0f,
		.y = bannerPos.y + 3.0f
	};
	union vec2 bannerDecorDim = {
		.x = bannerDim.x - 6.0f - 30.0f,
		.y = 3.0f
	};
	struct entity *bannerDecor = NewEntity(state, bannerDecorPos, bannerDecorDim, ENTTYPE_GENERICRECT);
	bannerDecor->rect.colour = COL_GREEN_DARK;
	
	union vec2 bannerNubPos = {
		.x = bannerPos.x + bannerDecorDim.x + 8.0f,
		.y = bannerPos.y + 3.0f
	};
	union vec2 bannerNubDim = {
		.x = 24.0f,
		.y = 24.0f
	};
	struct entity *bannerNub = NewEntity(state, bannerNubPos, bannerNubDim, ENTTYPE_GENERICRECT);	
	bannerNub->rect.colour = COL_GREEN_DARK;
	
	union vec2 updatePos = {
		.x = bannerDecorPos.x + 9.0f,
		.y = bannerPos.y + 7.0f
	};
	union vec2 updateDim = {
		.x = 100,
		.y = 25
	};
	struct entity *update = NewEntity(state, updatePos, updateDim, ENTTYPE_UPDATESTRING);
	BITSET(update->state, ENTSTATE_ISCLIPPED);
	update->clipRect = MakeClipRect(bannerPos.x + 10.0f, bannerPos.y, bannerDim.x - 34.0f, bannerDim.y);
	CreateLabel(update, "Type to Start\0", CHARSTATE_BLACK);	
}

static void CreateInput(
	struct game_state *state,
	struct screen_buffer *buffer)
{
	float xAnchor = (float)buffer->x * 0.25f;
	float yAnchor = (float)buffer->y - 80.0f;
	
	union vec2 inputBottomPos = {
		.x = xAnchor - 10.0f,
		.y = yAnchor - 185
	}; 
	union vec2 inputBottomDim = {
		.x = (float)buffer->x - 10 - xAnchor  - 30.0f,
		.y = 5.0f
	};
	struct entity *inputBottom = NewEntity(state, inputBottomPos, inputBottomDim, ENTTYPE_GENERICRECT);	
	BITSET(inputBottom->state, ENTSTATE_DRAWONCE);
	inputBottom->rect.colour = COL_GREEN;
	
	union vec2 inputTopPos = {
		.x = xAnchor - 10.0f, 
		.y = yAnchor - 150
	};
	union vec2 inputTopDim = {
		.x = buffer->x - inputTopPos.x - 10.0f - 40.0f,
		.y = 5.0f
	};
	struct entity *inputTop = NewEntity(state, inputTopPos, inputTopDim, ENTTYPE_GENERICRECT);	
	BITSET(inputTop->state, ENTSTATE_DRAWONCE);
	inputTop->rect.colour = COL_GREEN;
	
	union vec2 inputBackPos = {
		.x = (float)buffer->x * 0.25f, 
		.y = inputBottomPos.y + 5.0f
	};
	union vec2 inputBackDim = FloatToVec2((buffer->x - 50) - inputBackPos.x, 
		inputTopPos.y - inputBottomPos.y - 5.0f);
	struct entity *inputBack = NewEntity(state, inputBackPos, inputBackDim, ENTTYPE_GENERICRECT);	
	inputBack->rect.colour = COL_BLACK;
	
	union vec2 inputPos = {
		.x = inputBackPos.x,
		.y = inputBackPos.y + 7.0f,
	};
	union vec2 inputdim = inputBackDim;
	struct entity *inputControl = NewEntity(state, inputPos, inputdim, ENTTYPE_INPUTSTRING);	
	
	union vec2 cursorDim = FloatToVec2(5.0f, 20.0f);
	inputPos.y -= 2.0f;	
	struct entity *cursor = NewEntity(state, inputPos, cursorDim, ENTTYPE_CURSORRECT);
	cursor->rect.parentIndex = inputControl->index;
	cursor->rect.colour = COL_GREEN;
	struct entity_event * event = NewEvent(state, cursor->index, EVENT_BLINK);
	NewBlinkEvent(&state->timer, event);
}

extern void CreateUi(
	struct game_state *state,
	struct screen_buffer *buffer)
{
	float xAnchor = (float)buffer->x * 0.25f;
	float yAnchor = (float)buffer->y - 80.0f;
	float yAnchorInputTop = yAnchor - 150;
	float yAnchorInputBottom = yAnchor - 185;
	
	union vec2 backRectPos = {
		.x = 0,
		.y = 0
	};
	union vec2 backRectDim = {
		.x = bufferX,
		.y = bufferY
	};	
	struct entity *backRect = NewEntity(state, backRectPos, backRectDim, ENTTYPE_GENERICRECT);
	backRect->rect.colour = COL_BLACK;
	BITSET(backRect->state, ENTSTATE_DRAWONCE);
		
	CreateBanner(state, buffer);	
	CreateInput(state, buffer);	
	
	union vec2 topRectPos = {
		.x = bufferX + 10.0f,
		.y = bufferY - 15.0f
	};
	union vec2 topRectDim = {
		.x = bufferX - 20.0f,
		.y = 5.0f
	};	
	struct entity *topRect = NewEntity(state, topRectPos, topRectDim, ENTTYPE_GENERICRECT);
	topRect->rect.colour = COL_GREEN_DARK;
	BITSET(topRect->state, ENTSTATE_DRAWONCE);
	
	union vec2 headerPos = {
		.x = xAnchor, 
		.y = bufferY - 42.0f
	};
	union vec2 headerDim = {
		.x = 300.0f,
		.y = 25.0f
	};
	struct entity *header = NewEntity(state, headerPos, headerDim, ENTTYPE_LABELSTRING);	
	BITSET(header->state, ENTSTATE_DRAWONCE);		
	
	union vec2 outputRectPos = {
		.x = xAnchor,
		.y = yAnchor - 10 - 125
	};
	union vec2 outputRectDim = {
		.x = (float)bufferX - 20 - xAnchor,
		.y = 125
	};	
	struct entity *outputRect = NewEntity(state, outputRectPos, outputRectDim, ENTTYPE_GENERICRECT);
	outputRect->rect.colour = COL_BLACK;
	BITSET(outputRect->state, ENTSTATE_DRAWONCE);
			
	float yExtent = yAnchor- yAnchorInputTop - 20;
	float timerY = (yExtent - 25.0f) / 5;
	union vec2 timerDim = {
		.x = 30.0f,
		.y = timerY
	};
	for(int32_t i = 0; i < 5; i++) {		
		union vec2 timerPos = {
			.x = buffer->x - 40.0f,
			.y = (yAnchorInputTop + 15) + (timerY + 5) * i
		};
		struct entity *timer = NewEntity(state, timerPos, timerDim, ENTTYPE_GENERICRECT);	
		timer->rect.colour = COL_GREEN;		
	}		
		
	union vec2 nubPos = {
		.x = xAnchor - 10.0f - 40.0f,
		.y = yAnchor
	};
	union vec2 nubDim = {
		.x = 30.0,
		.y = 30.0f
	};
	struct entity *nub = NewEntity(state, nubPos, nubDim, ENTTYPE_GENERICRECT);
	nub->rect.colour = COL_ORANGE;
	BITSET(nub->state, ENTSTATE_DRAWONCE);
	
	union vec2 notBannerBottomPos = {
		.x = 10.0f,
		.y = yAnchor
	};
	union vec2 notBannerBottomDim = {
		.x = nubPos.x - 20.0f,
		.y = 30.0f
	};
	struct entity *notBannerBottom = NewEntity(state, notBannerBottomPos, 
		notBannerBottomDim, ENTTYPE_GENERICRECT);
	notBannerBottom->rect.colour = COL_ORANGE;
	BITSET(notBannerBottom->state, ENTSTATE_DRAWONCE);
	
	union vec2 scoreBoardBackPos = {
		.x = 10.0f,
		.y = yAnchorInputBottom
	};
	union vec2 scoreBoardBackDim = {
		.x = nubPos.x - 20.0f,
		.y = yAnchor - scoreBoardBackPos.y
	};	
	struct entity *scoreBoardBack = NewEntity(state, scoreBoardBackPos, 
		scoreBoardBackDim, ENTTYPE_GENERICRECT);
	scoreBoardBack->rect.colour = COL_BLACK;
	BITSET(scoreBoardBack->state, ENTSTATE_DRAWONCE);
	
	union vec2 scoreBoardPos = {
		.x = 10.0f,
		.y = yAnchor - 5.0f
	};
	union vec2 scoreBoardDim = {
		.x = nubPos.x - 20.0f,
		.y = 3.0f
	};
	for(int32_t i = 0; i < 6; ++i) {
		scoreBoardPos.y -= 30.0f;
		struct entity *scoreBoard = NewEntity(state, scoreBoardPos, 
			scoreBoardDim, ENTTYPE_GENERICRECT);
		scoreBoard->rect.colour = COL_ORANGE_DARK;
		BITSET(scoreBoard->state, ENTSTATE_DRAWONCE);
	}
		
	union vec2 scoreLblPos = {
		.x = 10.0f + 5.0f,
		.y = yAnchor + 6.0f
	};
	union vec2 scoreLblDim = {
		.x = 100.0f,
		.y = 30.0f
	};
	struct entity *scoreLbl = NewEntity(state, scoreLblPos, 
		scoreLblDim, ENTTYPE_LABELSTRING);
	CreateLabel(scoreLbl, "High Scores\0", CHARSTATE_BLACK);
	BITSET(scoreLbl->state, ENTSTATE_DRAWONCE);
				
	union vec2 dividerTopLeftPos = {
		.x = xAnchor - 10.0f - 40.0f,
		.y = yAnchorInputTop + 15.0f
	};
	union vec2 dividerTopLeftDim = {
		.x = 5.0f,
		.y = notBannerBottomPos.y - yAnchorInputTop- 25.0f
	};
	struct entity *dividerTopLeft = NewEntity(state, dividerTopLeftPos, 
		dividerTopLeftDim, ENTTYPE_GENERICRECT);
	dividerTopLeft->rect.colour = COL_ORANGE;
	BITSET(dividerTopLeft->state, ENTSTATE_DRAWONCE);
	
	union vec2 dividerTopRightPos = {
		.x = xAnchor - 25.0f,
		.y = yAnchorInputTop + 15.0f
	};
	union vec2 dividerTopRightDim = {
		.x = 5.0f,
		.y = notBannerBottomPos.y - yAnchorInputTop - 25.0f
	};
	struct entity *dividerTopRight = NewEntity(state, dividerTopRightPos, 
		dividerTopRightDim, ENTTYPE_GENERICRECT);
	dividerTopRight->rect.colour = COL_ORANGE;
	BITSET(dividerTopRight->state, ENTSTATE_DRAWONCE);
	
	union vec2 farNubPos = {
		.x = (float)buffer->x - 40.0f,
		.y = yAnchorInputBottom
	};
	union vec2  farNubDim = {
		.x = 30.0f,
		.y = yAnchorInputTop - yAnchorInputBottom + 6.0f
	};
	struct entity *farNub = NewEntity(state, farNubPos, 
		farNubDim, ENTTYPE_GENERICRECT);
	farNub->rect.colour = COL_GREEN;
	BITSET(farNub->state, ENTSTATE_DRAWONCE);
	
	union vec2 lowerNubPos = {
		.x = xAnchor - 10.0f - 40.0f,
		.y = yAnchorInputBottom
	};
	union vec2  lowerNubDim = {
		.x = 30.0f,
		.y = yAnchorInputTop - yAnchorInputBottom + 6.0f
	};
	struct entity *lowerNub = NewEntity(state, lowerNubPos, 
		lowerNubDim, ENTTYPE_GENERICRECT);
	lowerNub->rect.colour = COL_ORANGE;
	BITSET(lowerNub->state, ENTSTATE_DRAWONCE);
	
	union vec2 pgUpPos = {
		.x = xAnchor,
		.y = yAnchorInputBottom - 35.0f
	};
	union vec2  pgUpDim = {
		.x = 30.0f,
		.y = yAnchorInputTop - yAnchorInputBottom + 6.0f
	};
	struct entity *pgUp = NewEntity(state, pgUpPos, 
		pgUpDim, ENTTYPE_LABELSTRING);
	BITSET(pgUp->state, ENTSTATE_DRAWONCE);
	CreateLabel(pgUp, "PgUp  : Scroll Up\0", CHARSTATE_UI);
	
	pgUpPos.y -= 35.0f;
	struct entity *pgDown = NewEntity(state, pgUpPos, 
		pgUpDim, ENTTYPE_LABELSTRING);
	BITSET(pgDown->state, ENTSTATE_DRAWONCE);
	CreateLabel(pgDown, "PgDn  : Scroll Down\0", CHARSTATE_UI);
	
	pgUpPos.y -= 35.0f;
	struct entity *tab = NewEntity(state, pgUpPos, 
		pgUpDim, ENTTYPE_LABELSTRING);
	BITSET(tab->state, ENTSTATE_DRAWONCE);
	CreateLabel(tab, "Tab   : Restart\0", CHARSTATE_UI);
	
	union vec2 wpmRectPos = {
		.x = xAnchor + 250.0f,
		.y = yAnchorInputBottom - 25.0f - 15.0f
	};
	union vec2  wpmRectDim = {
		.x = 60.0f,
		.y = 30.0f
	};
	struct entity *wpmRect = NewEntity(state, wpmRectPos, 
		wpmRectDim, ENTTYPE_GENERICRECT);
	BITSET(wpmRect->state, ENTSTATE_DRAWONCE);	
	wpmRect->rect.colour = COL_GREEN;
	
	union vec2 wpmLblPos = {
		.x = xAnchor + 255.0f,
		.y = yAnchorInputBottom - 35.0f
	};
	union vec2  wpmLblDim = {
		.x = 100.0f,
		.y = 25.0f
	};
	struct entity *wpmLbl = NewEntity(state, wpmLblPos, 
		wpmLblDim, ENTTYPE_LABELSTRING);
	CreateLabel(wpmLbl, "WPM:\0", CHARSTATE_BLACK);
	BITSET(wpmLbl->state, ENTSTATE_DRAWONCE);	
	
	union vec2 accRectPos = {
		.x = xAnchor + 250.0f,
		.y = yAnchorInputBottom - 25.0f - 50.0f
	};
	union vec2  accRectDim = {
		.x = 60.0f,
		.y = 30.0f
	};
	struct entity *accRect = NewEntity(state, accRectPos, 
		accRectDim, ENTTYPE_GENERICRECT);
	BITSET(accRect->state, ENTSTATE_DRAWONCE);	
	accRect->rect.colour = COL_GREEN;
	
	union vec2 accLblPos = {
		.x = xAnchor + 255.0f,
		.y = yAnchorInputBottom - 67.5f
	};
	union vec2  accLblDim = {
		.x = 100.0f,
		.y = 25.0f
	};
	struct entity *accLbl = NewEntity(state, accLblPos, 
		accLblDim, ENTTYPE_LABELSTRING);
	CreateLabel(accLbl, "ACC:\0", CHARSTATE_BLACK);
	BITSET(accLbl->state, ENTSTATE_DRAWONCE);	
	
	union vec2 funRectPos = {
		.x = xAnchor + 250.0f,
		.y = yAnchorInputBottom - 25.0f - 85.0f
	};
	union vec2  funRectDim = {
		.x = 60.0f,
		.y = 30.0f
	};
	struct entity *funRect = NewEntity(state, funRectPos, 
		funRectDim, ENTTYPE_GENERICRECT);
	BITSET(funRect->state, ENTSTATE_DRAWONCE);	
	funRect->rect.colour = COL_GREEN;
	
	union vec2 funLblPos = {
		.x = xAnchor + 255.0f,
		.y = yAnchorInputBottom - 102.5f
	};
	union vec2  funLblDim = {
		.x = 100.0f,
		.y = 25.0f
	};
	struct entity *funLbl = NewEntity(state, funLblPos, 
		funLblDim, ENTTYPE_LABELSTRING);
	CreateLabel(funLbl, "FUN:\0", CHARSTATE_BLACK);
	BITSET(funLbl->state, ENTSTATE_DRAWONCE);	
	
	union vec2 scoreRectPos = {
		.x = xAnchor + 255.0f + 75.0f,
		.y = yAnchorInputBottom - 75.5f
	};
	union vec2  scoreRectDim = {
		.x = 60.0f,
		.y = 66.0f
	};
	struct entity *scoreRect = NewEntity(state, scoreRectPos, 
		scoreRectDim, ENTTYPE_GENERICRECT);
	BITSET(scoreRect->state, ENTSTATE_DRAWONCE);	
	scoreRect->rect.colour = COL_BLACK;
	
	union vec2 wpmScorePos = {
		.x = xAnchor + 255.0f + 75.0f,
		.y = yAnchorInputBottom - 35.0f
	};
	union vec2  wpmScoreDim = {
		.x = 100.0f,
		.y = 25.0f
	};
	struct entity *wpmScore = NewEntity(state, wpmScorePos, 
		wpmScoreDim, ENTTYPE_WPMSTRING);
	CreateLabel(wpmScore, "0\0", CHARSTATE_UI);
	BITSET(wpmScore->state, ENTSTATE_DRAWONCE);

	union vec2 accScorePos = {
		.x = xAnchor + 255.0f + 75.0f,
		.y = yAnchorInputBottom - 67.5f
	};
	union vec2  accScoreDim = {
		.x = 100.0f,
		.y = 25.0f
	};
	struct entity *accScore = NewEntity(state, accScorePos, 
		accScoreDim, ENTTYPE_ACCSTRING);
	CreateLabel(accScore, "0%\0", CHARSTATE_UI);
	BITSET(accScore->state, ENTSTATE_DRAWONCE);	
	
	union vec2 funScorePos = {
		.x = xAnchor + 255.0f + 75.0f,
		.y = yAnchorInputBottom - 102.5f
	};
	union vec2  funScoreDim = {
		.x = 100.0f,
		.y = 25.0f
	};
	struct entity *funScore = NewEntity(state, funScorePos, 
		funScoreDim, ENTTYPE_LABELSTRING);
	CreateLabel(funScore, "0\0", CHARSTATE_UI);
	BITSET(funScore->state, ENTSTATE_DRAWONCE);	
	
	union vec2 wpmSubBarPos = {
		.x = xAnchor + 400.0f,
		.y = yAnchorInputBottom - 25.0f - 15.0f
	};
	union vec2  wpmSubBarDim = {
		.x = bufferX - wpmSubBarPos.x - 50.0f,
		.y = 3.0f
	};
	struct entity *wpmSubBar = NewEntity(state, wpmSubBarPos, 
		wpmSubBarDim, ENTTYPE_GENERICRECT);
	BITSET(wpmSubBar->state, ENTSTATE_DRAWONCE);	
	wpmSubBar->rect.colour = COL_GREEN_DARK;
	
	union vec2 accSubBarPos = {
		.x = xAnchor + 400.0f,
		.y = yAnchorInputBottom - 25.0f - 50.0f
	};
	union vec2  accSubBarDim = {
		.x = bufferX - accSubBarPos.x - 50.0f,
		.y = 3.0f
	};
	struct entity *accSubBar = NewEntity(state, accSubBarPos, 
		accSubBarDim, ENTTYPE_GENERICRECT);
	BITSET(accSubBar->state, ENTSTATE_DRAWONCE);	
	accSubBar->rect.colour = COL_GREEN_DARK;
	
	union vec2 funSubBarPos = {
		.x = xAnchor + 400.0f,
		.y = yAnchorInputBottom - 25.0f - 85.0f
	};
	union vec2  funSubBarDim = {
		.x = bufferX - funSubBarPos.x - 50.0f,
		.y = 3.0f
	};
	struct entity *funSubBar = NewEntity(state, funSubBarPos, 
		funSubBarDim, ENTTYPE_GENERICRECT);
	BITSET(funSubBar->state, ENTSTATE_DRAWONCE);	
	funSubBar->rect.colour = COL_GREEN_DARK;
	
	union vec2 wpmBarPos = {
		.x = xAnchor + 400.0f,
		.y = yAnchorInputBottom - 25.0f - 12.0f
	};
	union vec2  wpmBarDim = {
		.x = 0,
		.y = 25.0f
	};
	struct entity *wpmBar = NewEntity(state, wpmBarPos, 
		wpmBarDim, ENTTYPE_GENERICRECT);	
	wpmBar->rect.colour = COL_GREEN;
	
	union vec2 wpmBarEndPos = {
		.x = xAnchor + 400.0f,
		.y = yAnchorInputBottom - 25.0f - 12.0f
	};
	union vec2  wpmBarEndDim = {
		.x = 20.0f,
		.y = 25.0f
	};
	struct entity *wpmBarEnd = NewEntity(state, wpmBarEndPos, 
		wpmBarEndDim, ENTTYPE_GENERICRECT);
	wpmBarEnd->rect.colour = COL_BLACK;
	
	union vec2 accBarPos = {
		.x = xAnchor + 400.0f,
		.y = yAnchorInputBottom - 25.0f - 47.0f
	};
	union vec2  accBarDim = {
		.x = 0,
		.y = 25.0f
	};
	struct entity *accBar = NewEntity(state, accBarPos, 
		accBarDim, ENTTYPE_GENERICRECT);	
	accBar->rect.colour = COL_GREEN;
	
	union vec2 accBarEndPos = {
		.x = xAnchor + 400.0f,
		.y = yAnchorInputBottom - 25.0f - 47.0f
	};
	union vec2  accBarEndDim = {
		.x = 20.0f,
		.y = 25.0f
	};
	struct entity *accBarEnd = NewEntity(state, accBarEndPos, 
		accBarEndDim, ENTTYPE_GENERICRECT);
	accBarEnd->rect.colour = COL_BLACK;
}
