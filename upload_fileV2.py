from fastapi import FastAPI,UploadFile, File, Request, Form
from fastapi.responses import FileResponse
from secrets import token_hex
from fastapi.responses import HTMLResponse
from fastapi.templating import Jinja2Templates
import os
app = FastAPI()
templates=Jinja2Templates(directory="htmldirectory")
path = "C:/python program"
global y
y="aku"
global versi2
versi2=0
@app.get("/home/", response_class=HTMLResponse)
def home(request: Request):
    return templates.TemplateResponse("index.html",{"request":request})


@app.get("/p/")
def index():
    global versi2
    print(versi2)
    return {"version": versi2,"url":"https://guarantee-prayer-ruby-distinct.trycloudflare.com/cat/"}

@app.get("/cat/")
def cat():
    global file_path
    file = os.path.join(path, file_path)
    if os.path.exists(file):
        return FileResponse(file, media_type="text", filename=file_path)
    return {"error" : "File not found!"}
    

@app.post("/submitform")
async def index2(versi:int=Form(...),file:UploadFile=File(...)):
    print(versi)
    global versi2
    global file_path
    versi2=versi
    #file_ext=file.filename.split(".").pop()
    file_name=token_hex(10)
    file_path=f"{file.filename}"
    with open(file_path,"wb")as f:
        content=await file.read()
        f.write(content)
    return{"sukses":True, "file_path":file_path}