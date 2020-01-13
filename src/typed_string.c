#define RANDOM_WORD_COUNT 30

static char *praiseStrings[] = {
	"Wow, I'm so impressed...", "Are you proud of yourself?", "Yeah, yeah, very good",
	"Get out more", "Careful now", "You'll get RSI", "Don't break the keyboard"
	"Think you're winning?"
}; 

static char *encouragementStrings[] = {
	"I'm not judging you", "I'm sure you have other skills", "What a good effort!",
	"Fine I guess", "Yeah! Good for you!", "You're normally better right?"
}; 

static char *headerStrings[] = {
	"Voted Worst Driving Simulator 5 Years Running",
	"Faster typing is only 234 Segfaults Away",
	"If it Crashes you're doing it Wrong",
};

static char templateStrings[3][800] = {
	{
		"I don't think he's %s the %s properly. Last time I saw him he was "					//2
		"%s putting the %s down the %s. That's just %s, %s, and to be frank, %s. There's so many "		//6
		"other %ss you can %s, that %s a %s is a %s waste of %s. I just want to %s him and "			//7
		"%s that this sort thing is, at best, %s, and at worst, %s. That's not even the most "			//3
		"%s thing I've seen him do this week. He's got this %s from somewhere, and started %s it "		//3
		"with %s. I asked why, and he just said \"this is the only way I can %s %s to get the %s off, "		//4
		"and I've nearly run out of %ss.\" I mean, what's %s %ss got to do with anything. I'm loosing " 	//3
		"faith with the whole thing at this point, he can %s his own %s.\0"					//2				//2
	}, {
		"\"Ever since she was young and %s, she was obsessed with %s %ss. Her parents %s lamented this, thinking "	//4
		"this wasn't the %s future they had planned, \"She should be %s %s or something %s like that!\" They "		//4
		"would %s say to each other. But nothing would stop her, and now, 20 %s, and %s years later, she's still %s "	//4
		"%s with the same %s as before, and has become a leading authority in the %s of %s. Her papers on the "	//4
		"%s %ss of the region have become some of the most %s in the field, and earned her the %s, %s %s "	//6
		"award. Try %s a %s with a CV like that. So please %s welcome our keynote speaker, the %s woman with the "	//4
		"hair weaved entirely from %s, and %s %ss!\", \"My %s thanks, Leslie.\"\0"					//4			//4
	}, {
		"Herbert looked into Audrey's %s eyes, and was instantly taken back many years, to one %s day. "	//2
		"Audrey had just finished %s some %ss, and he had hurried home %s so as not to miss the %s. "		//4
		"She was always %s with the %s, and it filled Herbert's heart with %s %s. As Audrey looked at Herbert, "	//4
		"she thought only of the %s. There were loads of them in those days, %s, %s, and %s, all over the "	//4
		"%s. Herbert %s tried his best to %s them before the whole %s was overrun, and she %sed like a %s "	//6
		"while watching him try to %s with a %s. Happy days. She smiled, and asked, \"Are you thinking about "	//2
		"%s with %s again?\" He smiled, \"You know me too well, it feels like only yesterday we were %s %s, "	//4
		"and now I can't even %s %s a %s\". She beamed, her eyes slowly closing, \"Like a %s\", she said.\0"	//4
	},
};

/*
*	always 30 random words, no exceptions 
*/
enum string_slots templateStringSlots[][30] = {
	{
		SLOT_VERBING, SLOT_NOUN, 
		SLOT_ADJLY, SLOT_NOUN, SLOT_NOUN, SLOT_ADJ, SLOT_ADJ, SLOT_ADJ,
		SLOT_NOUN, SLOT_VERB, SLOT_VERBING, SLOT_NOUN, SLOT_ADJ, SLOT_NOUN, SLOT_VERB, 
		SLOT_VERB, SLOT_ADJ, SLOT_ADJ, 
		SLOT_ADJ, SLOT_NOUN, SLOT_VERBING,
		SLOT_NOUN, SLOT_ADJLY, SLOT_VERB, SLOT_NOUN, 
		SLOT_NOUN, SLOT_VERBING, SLOT_NOUN, 
		SLOT_VERB, SLOT_NOUN
	}, {
		SLOT_ADJ, SLOT_VERBING, SLOT_NOUN, SLOT_ADJLY,
		SLOT_ADJ, SLOT_VERBING, SLOT_NOUN, SLOT_ADJ,
		SLOT_ADJLY, SLOT_ADJ, SLOT_ADJ, SLOT_VERBING,
		SLOT_NOUN, SLOT_NOUN, SLOT_VERBING, SLOT_NOUN,
		SLOT_ADJ, SLOT_NOUN, SLOT_ADJ, SLOT_ADJ, SLOT_ADJ, SLOT_NOUN,
		SLOT_VERBING, SLOT_NOUN, SLOT_ADJLY, SLOT_ADJ,
		SLOT_NOUN, SLOT_ADJ, SLOT_NOUN, SLOT_ADJ
	},  {
		SLOT_ADJ, SLOT_ADJ, 
		SLOT_VERBING, SLOT_NOUN, SLOT_ADJLY, SLOT_NOUN,
		SLOT_VERBING, SLOT_NOUN, SLOT_ADJ, SLOT_NOUN,
		SLOT_NOUN, SLOT_VERBING, SLOT_VERBING, SLOT_VERBING,
		SLOT_NOUN, SLOT_ADJLY, SLOT_VERB, SLOT_NOUN, SLOT_VERB, SLOT_NOUN,
		SLOT_VERB, SLOT_NOUN,
		SLOT_VERBING, SLOT_NOUN, SLOT_ADJLY, SLOT_VERBING,
		SLOT_ADJLY, SLOT_VERB, SLOT_NOUN, SLOT_NOUN		
	},
};	

static char *nouns[] = {
	"sink", "heron", "Dolmio", "pasta", "toilet", "puppet", "beetle", "towel",
	"idea", "saucepan", "dishcloth", "lemon", "salmon", "WD40", "screwdriver", "trousers",
	"dog", "parrot", "bicycle", "pot plant", "emotion", "emulsion", "paint", "eiderdown",
	"biscuit", "glove", "hoover", "chicken", "pony", "bath", "fishing rod", "peanut", 
	"furniture", "crash mat", "snooker cue", "unwashed masses", "washing line", "epilator",
	"yard", "mud", "hamster", "cake", "snot", "wendy house", "mallet", "tent", "caramel",
	"foam", "perfume", "cardigan", "cowboy boot", "cheese knife", "tomato"
};

static char *ingVerbs[] = {
	"rubbing", "fishing", "farming", "carrying", "asking", "questioning", "keeping", "drink",
	"icing", "dressing", "picking", "working", "licking", "kissing", "hiding", "filing",
	"riding", "running", "walking", "stopping", "tickling", "cooking", "preparing",
	"waxing", "plucking", "shaving", "agitating", "frightening", "explaining", "saying",
	"smelling", "annoying", "decanting", "sailing", "poking", "combing", "painting"
};

static char *verbs[] = {
	"rub", "fish", "farm", "carry", "ask", "question", "keep", "drink",
	"ice", "dress", "pick", "work", "lick", "kiss", "hide", "file",
	"ride", "run", "walk", "stop", "tickle", "cook", "prepare",
	"wax", "pluck", "shave", "agitate", "frighten", "explain", "say",
	"smell", "annoy", "decant", "sail", "poke", "comb", "paint"
};

static char *adjectives[] = {
	"green", "sad", "hopeful", "joyous", "pathetic", "slimey", "wretched", "delerious",
	"shiney", "glossy", "dull", "painful", "vague", "naive", "innocent", "vulgar",
	"stoic", "extatic", "ill-advised", "homely", "beautiful", "ropey", "spicey",
	"doomed", "charismatic", "funny", "hilarious", "jubilant", "wanton", "unnecessary",
	"interesting", "disquieting", "stupid", "reckless", "sheepish"
};

static char *lyAdjectives[] = {
	"quickly", "safely", "practically", "smoothly", "roughly", "gently", "softly", 
	"hygeinically", "cleanly", "gingerly", "ruthlesly", "carefully",
};

static void CreateOutputString(
	struct game_state *state)
{
	uint32_t inputIndex = rand() % 3;
	char *string = &templateStrings[inputIndex][0];
	
	uint32_t wordCount = RANDOM_WORD_COUNT;
	char *insertedWords[wordCount];
	size_t charCount = strlen(string);
	
	for(int32_t i = 0; i < wordCount; ++i) {
		switch(templateStringSlots[inputIndex][i]) {
		case SLOT_ADJ: {
			insertedWords[i] = adjectives[rand() % ARRAY_COUNT(adjectives)];
			break;
		} case SLOT_ADJLY: {
			insertedWords[i] = lyAdjectives[rand() % ARRAY_COUNT(lyAdjectives)];
			break;
		} case SLOT_NOUN: {
			insertedWords[i] = nouns[rand() % ARRAY_COUNT(nouns)];
			break;
		} case SLOT_VERB: {
			insertedWords[i] = verbs[rand() % ARRAY_COUNT(verbs)];
			break;
		} case SLOT_VERBING: {
			insertedWords[i] = ingVerbs[rand() % ARRAY_COUNT(ingVerbs)];
			break;
		} default: INVALID_PATH;
		}
		charCount += strlen(insertedWords[i]);
	} 
	state->outputStringBuffer = api.Allocate(charCount);
	state->outputStringBufferSize = charCount;
	
	sprintf(state->outputStringBuffer, string,
		insertedWords[0], insertedWords[1], insertedWords[2], insertedWords[3],
		insertedWords[4], insertedWords[5], insertedWords[6], insertedWords[7],
		insertedWords[8], insertedWords[9], insertedWords[10], insertedWords[11],
		insertedWords[12], insertedWords[13], insertedWords[14], insertedWords[15],
		insertedWords[16], insertedWords[17], insertedWords[18], insertedWords[19],
		insertedWords[20], insertedWords[21], insertedWords[22], insertedWords[23],
		insertedWords[24], insertedWords[25], insertedWords[26], insertedWords[27],
		insertedWords[28], insertedWords[29]);	
}

extern void CreateLabel(
	struct entity *newEntity,
	char *content,
	enum character_state state)
{
	int32_t i = 0;
	for(char *current = content; *current != '\0'; ++current, ++i) {
		newEntity->string.contents[i].glyph = *current;		
		newEntity->string.contents[i].state = state;		
	}
	newEntity->string.length = strlen(content);
}

extern void CreateScore(
	struct entity *newEntity,
	uint32_t score,
	uint32_t position,
	enum character_state state)
{
	char buffer[10];
	sprintf(buffer, "%d,    %d", position, score);
	
	CreateLabel(newEntity, buffer, state);
	newEntity->string.score = score;
	newEntity->string.position = position;
}

extern void ConvertString(
	char *start,
	char *end,
	struct entity_string *entity,
	enum character_state state)
{
	entity->length = end - start;
	assert(entity->length < ARRAY_COUNT(entity->contents));
	char *current = start;
	for(int32_t i = 0; i < entity->length; ++i, ++current) {
		entity->contents[i].glyph = *current;
		entity->contents[i].state = state;
	}
}

extern void ChangeLabelState(
	struct entity *label,
	enum character_state state)
{	
	for(int32_t i = 0; i != label->string.length; ++i) {	
		label->string.contents[i].state = state;		
	}
}

extern uint32_t GetCharacterWidth(
	struct game_state *state,	
	char glyph)
{
	uint32_t result;
	if(glyph == ' ') {
		result = SPACE_WIDTH;			
	} else {	
		struct loaded_character *character = 
			GetCharacter(glyph, state->characterBuffer);
		result = character->x * SCALE + 1.0f;
	}	
	return(result);
}

extern void PrepAccString(
	struct game_state *state,
	struct entity *current)
{
	char buffer[12];
	sprintf(buffer, "%.0f%%", state->score.accuracey);
	CreateLabel(current, buffer, CHARSTATE_UI);
}

extern void PrepWpmString(
	struct game_state *state,
	struct entity *current)
{
	char buffer[12];
	sprintf(buffer, "%d", state->score.wpm);
	CreateLabel(current, buffer, CHARSTATE_UI);
}

extern void InitNewLine(
	struct entity *line,
	struct rect2 clipRect)
{
	BITSET(line->state, ENTSTATE_ISCLIPPED);
	line->clipRect = clipRect;
	line->string.backgroundCount = 1;
}

static void CreateOutputEntities(
	struct game_state *state,
	uint32_t screenX,
	uint32_t screenY)
{
	CreateOutputString(state);
	
	float xAnchor = (float)bufferX * 0.25f;
	float yAnchor = (float)bufferY - 80.0f;	
	
	struct rect2 outputClipRect = MakeClipRect(
		xAnchor, yAnchor - 10 - 125,
		(float)bufferX - 20 - xAnchor, 125);
	
	union vec2 pos = FloatToVec2(
		(float)screenX * 0.25f,
		(float)screenY - 110.0f);
	union vec2 dim = FloatToVec2(
		(screenX - 50) - pos.x - 25.0f,
		25.0f);		
	
	struct entity *newEntity = NewEntity(state, pos, dim, ENTTYPE_OUTPUTSTRING);	
	InitNewLine(newEntity, outputClipRect);
		
	uint32_t totalines = 0;		
	uint32_t overflow = dim.x - 60.0f;
	char *startFrom = state->outputStringBuffer;
	char *current = startFrom;
	char *lastSpace = startFrom;
	uint32_t length = state->outputStringBufferSize;
	for(int32_t i = 0; i < length; ++i, ++current) {		
		if(*current != '\0') {
			newEntity->string.lengthInPixels += 
				GetCharacterWidth(state, *current);
		}		
		if(newEntity->string.lengthInPixels > overflow) {	
			length +=  current - lastSpace;
			ConvertString(startFrom, lastSpace, &newEntity->string, CHARSTATE_NEUTRAL);
			pos.y -= 25.0f;
			newEntity = NewEntity(state, pos, dim, ENTTYPE_OUTPUTSTRING);	
			InitNewLine(newEntity, outputClipRect);
			newEntity->string.position = ++totalines;	
			startFrom = ++lastSpace;
			current = startFrom;
			continue;
		} 		
		if(*current == ' ') {			
			lastSpace = current;
		} else if(*current == '\0') {
			ConvertString(startFrom, current, &newEntity->string, CHARSTATE_NEUTRAL);
			state->outputLines = totalines; 	
			break;
		}
	}
}
