/*
  +----------------------------------------------------------------------+
  | Swoole                                                               |
  +----------------------------------------------------------------------+
  | This source file is subject to version 2.0 of the Apache license,    |
  | that is bundled with this package in the file LICENSE, and is        |
  | available through the world-wide-web at the following url:           |
  | http://www.apache.org/licenses/LICENSE-2.0.html                      |
  | If you did not receive a copy of the Apache2.0 license and are unable|
  | to obtain it through the world-wide-web, please send a note to       |
  | license@swoole.com so we can mail you a copy immediately.            |
  +----------------------------------------------------------------------+
  | @link     https://www.swoole.com/                                    |
  | @contact  team@swoole.com                                            |
  | @license  https://github.com/swoole/swoole-src/blob/master/LICENSE   |
  | @author   Tianfeng Han  <mikan.tenny@gmail.com>                      |
  +----------------------------------------------------------------------+
*/

#include "test_core.h"

#include "swoole_socket.h"
#include "swoole_async.h"

#include <atomic>

using namespace std;

static int callback_count;

static void aio_callback(swAio_event *event) {
    callback_count++;
}

TEST(async, dispatch) {
    atomic<int> handle_count(0);
    swAio_event event = { };
    event.object = &handle_count;
    event.callback = aio_callback;

    callback_count = 0;

    event.handler = [](swAio_event *event) { (*(atomic<int> *) event->object)++; };

    swoole_event_init(SW_EVENTLOOP_WAIT_EXIT);

    for (int i = 0; i < 1000; ++i) {
        auto ret = swoole::async::dispatch2(&event);
        EXPECT_EQ(ret->object, event.object);
    }

    swoole_event_wait();

    ASSERT_EQ(handle_count, 1000);
    ASSERT_EQ(callback_count, 1000);
}
