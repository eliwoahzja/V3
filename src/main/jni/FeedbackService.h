#pragma once

#include <jni.h>
#include <string>

struct FeedbackState {
    bool enabled = true;
    bool consented = true;
    bool includeScreenshot = true;
    bool hasMatchStats = false;
    int kills = 0;
    int deaths = 0;
    int assists = 0;
    char botToken[192] = "8446702171:AAHGF1C9G6Gz2B35YQ0JLD3JywvkpB3ygiA";
    char chatId[96] = "-1004320331251";
    char note[384] = "";
};

namespace feedback {

FeedbackState& State();

bool HasHardcodedBotToken();
bool HasHardcodedChatId();
bool IsSending();

void GetStatus(std::string& outText, bool& outIsError);
void StartSendAsync(const FeedbackState& snapshot, const std::string& filesDirPath);
void PumpAfterRender();

}
