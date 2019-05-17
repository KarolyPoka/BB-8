# BB-8 <img align="left" src="images/bb8.png" width="158" height="254"><img align="right" src="images/bb8_ours.jpg" width="241" height="214">

## What is this repo for

In this repo I will show You how to set-up the BB-8 droid. You could download the appropriate file which is operating the robot. You will have a brief introduction to assemble the system and connect the mobile application with the ESP-32 modul BB-8 inside.


### Prerequisites


What things you need to upload the control software to BB-8 and how to get control above it.


- Arudino [IDE](https://www.arduino.cc/en/Main/Software/)
- ESP-32 hardware folder for the IDE[link in the future]
- Blynk Andorid [App](https://play.google.com/store/apps/details?id=cc.blynk&hl=en_US)


### Installing

You can follow these steps to be able to use the droid:

- Download the Blynk app to you mobile phone using the link above.

- Create the following board with the following settings:
<img align="left" src="images/blynk_idle.jpg" width="158" height="254">

- Upload the body-s controller conde to the ESP module with the Ardunio IDE paying attention to the following code parts:

```
//example 
char auth[] = "4191ba0acfbb4d3583f420c3d9d0ef1e"; //specific for every Blynk project and user (You can create it with the Blink app)
char ssid[] = "J.A.R.V.I.S.";                     //your hotspot's wifi name
char pass[] = "tonystarkdiesinendgame";           //your hotspot's wifi password
```
- Connect the power source to the Body's motherboard and wait for the ESP-Mobile connection. If the connection is successfull the 
little red sing will disappear in the Blynk' board.

- Try to control the motors.



End with an example of getting some data out of the system or using it for a little demo

## Running the tests

Explain how to run the automated tests for this system

### Break down into end to end tests

Explain what these tests test and why

```
Give an example
```

### And coding style tests

Explain what these tests test and why

```
Give an example
```

## Deployment

Add additional notes about how to deploy this on a live system

## Built With

* [Dropwizard](http://www.dropwizard.io/1.0.2/docs/) - The web framework used
* [Maven](https://maven.apache.org/) - Dependency Management
* [ROME](https://rometools.github.io/rome/) - Used to generate RSS Feeds

## Contributing

Please read [CONTRIBUTING.md](https://gist.github.com/PurpleBooth/b24679402957c63ec426) for details on our code of conduct, and the process for submitting pull requests to us.

## Versioning

We use [SemVer](http://semver.org/) for versioning. For the versions available, see the [tags on this repository](https://github.com/your/project/tags). 

## Authors

* **Karoly Poka** :godmode: - *BSc-thesis* - electrical engieneering-control systems :electric_plug:

See also the list of [contributors](https://github.com/your/project/contributors) who participated in this project.

## License
Without my marvelous hardware this repo does not worth a shit to You but if it does..."WE HAVE A HULK” .

## Acknowledgments

* Myself
* Star Wars movie
* Dr. Tevesz Gábor: Robotirányítás rendszertechnikája, Elektornikus jegyzet, 6. fejezet, 2017
* Lantos Béla-Irányítási Rendszerek Elmélete és Tervezése I. Egyváltozós szabályozások (Akadémia Kiadó, Budapest)
