#include "IoService.h"

namespace Http::Asio {

IoServiceBoost& IoService::getIoService() {
	return mIoService;
}

} // namespace Http::Asio