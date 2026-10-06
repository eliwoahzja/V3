#include "FeedbackService.h"

#include <mutex>
#include <string>

namespace {

std::mutex g_StatusMutex;
std::string g_Status = "Disabled";
bool g_StatusIsError = false;

}

namespace feedback {

void GetStatus(std::string& outText, bool& outIsError) {
    std::lock_guard<std::mutex> lock(g_StatusMutex);
    outText = g_Status;
    outIsError = g_StatusIsError;
}

}
