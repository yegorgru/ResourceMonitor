from fastapi import FastAPI, Request
from fastapi.responses import JSONResponse
import uvicorn
import config
import asyncio # needed if want to add delays etc.

app = FastAPI(title="Simple Async Server")

CONSTANT_RESPONSE = {"status": "OK", "message": "This is a constant response."}

@app.middleware("http")
async def catch_all_middleware(request: Request, call_next):
    return JSONResponse(content=CONSTANT_RESPONSE)

if __name__ == "__main__":
    print(f"Starting Uvicorn server on http://{config.HOST}:{config.PORT}")
    uvicorn.run("mock:app", host=config.HOST, port=config.PORT, reload=True)