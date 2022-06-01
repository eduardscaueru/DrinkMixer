from flask import Flask, render_template, Response, request
import paho.mqtt.client as mqtt
import time

app = Flask(__name__)
num_requests = 0
broker_address = "test.mosquitto.org"

def on_connect(client, userdata, flags, rc):
    global connected
    if rc == 0:
        print("connected OK Returned code = ", rc)
        client.connected_flag = True
        # print("Subscribing to topic", "mixer/drink")
        # client.subscribe("mixer/drink")
    else:
        print("Bad connection Returned code = ", rc)

def on_log(client, userdata, level, buf):
    print("log: ",buf)


def ShirleyGinger():
    msg = "SG"
    client_mqtt.publish("mixer/drink", msg, 0)
    print("Shirley Ginger")


def VirginCucumberGimlet():
    msg = "VCG"
    client_mqtt.publish("mixer/drink", msg, 0)
    print("Virgin Cucumber Gimlet")

def Lemonade():
    msg = "LEM"
    client_mqtt.publish("mixer/drink", msg, 0)
    print("Lemonade")

def StrawberryLemonade():
    msg = "SL"
    client_mqtt.publish("mixer/drink", msg, 0)
    print("Strawberry Lemonade")


@app.route("/", methods=['GET', 'POST'])
def index():
    global num_requests
    num_requests += 1
    if num_requests == 1:
        return render_template('index.html')

    if request.method == 'POST':
        print(request.form.to_dict())
        if 'SG' in request.form.to_dict():
            ShirleyGinger()
            # return render_template('test.html')
        if 'VCG' in request.form.to_dict():
            VirginCucumberGimlet()
        if 'LEM' in request.form.to_dict():
            Lemonade()
        if 'SL' in request.form.to_dict():
            StrawberryLemonade()
        # if 'off' in request.form.to_dict():
        #     return render_template('test.html')
    # if 'dis' in request.form.to_dict():
    # 	disconnect()

    # if request.form['off']:
    #	print('asdfdsafsdfasf')
    return render_template('index.html')


@app.route("/test", methods=['GET', 'POST'])
def test():
    # if request.method == 'POST':
    # 	if 'on' in request.form.to_dict():
    # 		ledOn()
    # 	if 'off' in request.form.to_dict():
    # 		ledOff()
    # 	if 'dis' in request.form.to_dict():
    # 		disconnect()
    #
    # 	#if request.form['off']:
    # 	#	print('asdfdsafsdfasf')
    return render_template('test.html')


if __name__ == '__main__':
    mqtt.Client.connected_flag = False
    client_mqtt = mqtt.Client("client1")
    client_mqtt.on_connect = on_connect
    # client.on_message = on_message
    client_mqtt.on_log = on_log

    client_mqtt.loop_start()

    print("Connecting to broker ", broker_address)
    client_mqtt.connect(broker_address)
    while not client_mqtt.connected_flag:
        print("In wait loop")
        time.sleep(1)

    app.run()
