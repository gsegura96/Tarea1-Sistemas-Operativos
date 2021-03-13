import os
import requests
from flask import Flask, flash, request, redirect, url_for, render_template

app = Flask(__name__, template_folder="client")
app.secret_key = b'_5#y2L"F4Q8z\n\xec]/'


@app.route('/')
def hello():
  return render_template("index.html")


@app.route('/', methods=['POST'])
def upload_file():
  host = request.form.get('host', 'http://localhost')
  port = request.form.get('port', '1717')
  file = request.files.get('file', None)
  # if user does not select file, browser also
  # submit an empty part without filename
  if not file or file.filename == '':
    flash('No se ha subido ninguna imagen.')
    return redirect(request.url)
  print(f'{host}:{port}')

  try:
    requests.post(url=f'{host}:{port}',
                  data=file.read(),
                  headers={'Content-Type': 'application/x-www-form-urlencoded'})
  except:
    flash('No se ha podido subir la imagen')
    return redirect(request.url)

  flash('La imagen se ha subido y se está procesando.')
  return redirect(request.url)


if __name__ == '__main__':
    app.run()
