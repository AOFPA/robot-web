//express
const express = require('express');
const bodyParser = require('body-parser');
const app = express();
const port_server = 3000;

app.use(bodyParser.json());
app.use(bodyParser.urlencoded({ extended: true }));
// .express

const cors = require('cors');
app.use(cors());



//MQTT
const mqtt = require('mqtt');
const port = '1883'
const clientId = `mqtt_${Math.random().toString(16).slice(3)}`
// coeiot:ComputerEng@2021
const mqtt_server = '203.158.177.150'
const mqtt_user = 'coeiot'
const mqtt_pwd = 'ComputerEng@2021'

const connectUrl = `http://${mqtt_server}:${port}`
//END MQTT

const client = mqtt.connect(connectUrl, {
    clientId,
    clean: true,
    connectTimeout: 4000,
    username: mqtt_user,
    password: mqtt_pwd,
    reconnectPeriod: 1000,
})

const topic = '/nodejs/mqtt'
const topic_tb1 = 'table1'
const topic_tb2 = 'table2'

client.on('connect', () => {
    console.log('Connected')

    client.subscribe([topic], () => {
        console.log(`Subscribe to topic '${topic}'`)
    })

    client.publish(topic, 'nodejs mqtt test', { qos: 0, retain: false }, (error) => {
        if (error) {
            console.error(error)
        }
    })
})

client.on('message', (topic, payload) => {
    console.log('Received Message:', topic, payload.toString())
})

app.get('/', (req, res) => {
    res.send('Hello World!')
})

let obj = `{
    order1: '1',
    order2: '2'
}`;

app.post("/table/:tb_id", function (req, res) {
    // mqttClient.sendMessage(req.body.message);
    let tb_id = req.params.tb_id;
    console.log(tb_id);
    client.publish('table', obj);
    res.status(200).send("Message sent to mqtt. topic: table1");
});

app.post('/post', (req, res) => {

    req.body;
    // res.json(req.body);
    res.status(200);
    res.json(req.body);
    console.log(req.body);

    const myJSON = JSON.stringify(req.body);
    client.publish('table', myJSON);
});


app.listen(port_server, () => {
    console.log("app running on port.", port_server);
})

