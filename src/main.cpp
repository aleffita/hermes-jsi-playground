#include <iostream>
#include <hermes/hermes.h>
#include <chrono>
#include <fstream>
#include <iostream>
#include <optional>
#include <sstream>
#include <thread>
#include <SFML/Window.hpp>


using namespace ::facebook;

static std::optional<std::string> readFile(const char *path) {
    std::ifstream fileStream(path);
    std::stringstream stringStream;

    if (fileStream) {
        stringStream << fileStream.rdbuf();
        fileStream.close();
    } else {
        // Handle error - file opening failed
        std::cerr << path << ": error opening file" << std::endl;
        return std::nullopt;
    }

    return stringStream.str();
}


int main() {

    sf::Window window(sf::VideoMode(sf::Vector2u(800, 600)), "My window");


    const char *jsPath = "/Users/alef/workdir/dino-engine/src/main.js";
    const char *jsLibPath = "/Users/alef/workdir/dino-engine/src/jslib.js";

    auto optCode = readFile(jsPath);
    if (!optCode)
        return 1;

    auto optCodeJsLib = readFile(jsLibPath);
    if (!optCode)
        return 1;

    auto runtimeConfig = ::hermes::vm::RuntimeConfig::Builder()
            .withIntl(false)
            .withMicrotaskQueue(true)
            .build();

    auto runtime = facebook::hermes::makeHermesRuntime(runtimeConfig);


    try {
        // Register event loop functions and obtain the runMicroTask() helper
        // function.
        jsi::Object helpers = runtime
                ->evaluateJavaScript(
                        std::make_unique<jsi::StringBuffer>(std::move(*optCodeJsLib)), "jslib.js.inc")
                .asObject(*runtime);

        jsi::Function peekMacroTask =
                helpers.getPropertyAsFunction(*runtime, "peek");

        jsi::Function runMacroTask = helpers.getPropertyAsFunction(*runtime, "run");

        // There are no pending tasks, but we want to initialize the event loop
        // current time.
        {
            double curTimeMs = (double) std::chrono::duration_cast<std::chrono::milliseconds>(
                    std::chrono::steady_clock::now().time_since_epoch())
                    .count();
            runMacroTask.call(*runtime, curTimeMs);
        }


        runtime->evaluateJavaScript(std::make_unique<jsi::StringBuffer>(std::move(*optCode)), jsPath);
        runtime->drainMicrotasks();

        double nextTimeMs;

        while (window.isOpen()) {
            // check all the window's events that were triggered since the last iteration of the loop
            sf::Event event;

            nextTimeMs = peekMacroTask.call(*runtime).getNumber();

            while (nextTimeMs >= 0 || runtime->drainMicrotasks()) {

                if(window.pollEvent(event)) {
                    if (event.type == sf::Event::Closed)
                    {
                        window.close();
                        break;
                    }
                }

                nextTimeMs = peekMacroTask.call(*runtime).getNumber();

                // This is the event loop. Loop while there are pending tasks.
                if (nextTimeMs >= 0) {
                    auto now = std::chrono::steady_clock::now();
                    double curTimeMs =
                            (double) std::chrono::duration_cast<std::chrono::milliseconds>(
                                    now.time_since_epoch())
                                    .count();

                    // If we have to, sleep until the next task is ready.
                    if (nextTimeMs > curTimeMs) {
                        continue;
                    }

                    // Run the next task.
                    runMacroTask.call(*runtime, curTimeMs);
                    runtime->drainMicrotasks();
                }
            }
        }
    } catch (jsi::JSError &e) {
        // Handle JS exceptions here.
        std::cerr << "JS Exception: " << e.getStack() << std::endl;
        return 1;
    } catch (jsi::JSIException &e) {
        // Handle JSI exceptions here.
        std::cerr << "JSI Exception: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
