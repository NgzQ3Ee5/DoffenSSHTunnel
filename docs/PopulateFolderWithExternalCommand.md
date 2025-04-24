On a folder insert the path to an external command/script that prints a json host list to stdout.
This is a real example of a script that queries the Amazon AWS API and inserts all EC2 servers into the folder. 
When double clicking the folder the command executes and the folder contents is updated. 

**First prepare a readonly profile for AWS credentials**
https://docs.aws.amazon.com/sdk-for-javascript/v2/developer-guide/loading-node-credentials-shared.html

~~~
mkdir ~/.aws
nano ~/.aws/credentials 
~~~

Insert
~~~
[readonly] ; read only access
aws_access_key_id = <YOUR_ACCESS_KEY_ID>
aws_secret_access_key = <YOUR_SECRET_ACCESS_KEY>
~~~

~~~
chmod 600 ~/.aws/credentials 
~~~

**This is the command set on the folder (in the edit pane)**

~~~
$node $myfolder/nodejs/ae_aws_ec2_instances.js readonly eu-west-1
~~~


**ae_aws_ec2_instances.js**

~~~
// User double clicked or pressed Enter on the AWS Instances folder in Doffen SSH and that caused this script to execute.

var AWS = require('aws-sdk'); //http://docs.aws.amazon.com/AWSJavaScriptSDK/latest/AWS/EC2.html#describeInstances-property
var S = require('string');
var moment = require('moment'); //http://momentjs.com/

var args = process.argv.slice(2);
if(args.length != 2) {
	console.log("Invalid arguments");
	return 1;
}

var argAwsProfile = args[0];
var argRegion = args[1];

//AWS.config.update({accessKeyId: argAccessKeyId, secretAccessKey: argSecretAccessKey});
var credentials = new AWS.SharedIniFileCredentials({profile: argAwsProfile});
AWS.config.credentials = credentials;
AWS.config.region = argRegion;

var ec2 = new AWS.EC2({apiVersion: '2015-04-15'});
ec2.describeInstances({}, function(err, data) {
  if (err) {
	// an error occurred
	console.log(err, err.stack); 
	process.exit(1);
  } else {
	//console.log("%j",data);
	//return;
	// successful response
	//These hosts will be inserted as children in the AWS Instances folder in Doffen SSH
	var dstHostsRunning = []; 
	var dstHostsNotRunning = [];
	var counter = 0;
	for(var i in data["Reservations"]) {
		var Reservation = data["Reservations"][i];
		for(var j in Reservation["Instances"]) {
			//Doffen SSH limits its log view to 8KB (tail). If I have many ec2 instances and I if I can't 
			//see the whole json document that I print in this script in the Doffen SSH log window
			//then I just uncomment the following line during development of this script.
			//if(counter == 5) break;
			var Instance = Reservation["Instances"][j];
			
			var stateName = Instance["State"]["Name"];
			var tagName = "";
			var tagEnvironment = "";
			var tagUser = "ubuntu"; //default ssh term user
			for(var k in Instance["Tags"]) {
				var Tag = Instance["Tags"][k]
				if(Tag["Key"] == "Name") {
					tagName = Tag["Value"];
				}
				if(Tag["Key"] == "Environment") {
					tagEnvironment = Tag["Value"];
				}
				if(Tag["Key"] == "User") {
					tagUser = Tag["Value"];
				}
			}
			var pubIp = Instance["PublicIpAddress"];
			var privIp = Instance["PrivateIpAddress"];
			var keyName = Instance["KeyName"]
			var launchTime = Instance["LaunchTime"]; //Date
			var runningTimeStr = moment(launchTime).fromNow(true); //4 days
			
			//If the ec2 has no name then use the instance id as name
			if(S(tagName).trim().isEmpty()) {
				tagName = Instance["InstanceId"]
			}
			
			var fgColor = "";
			if(stateName != "running") {
				//http://doc.qt.io/qt-4.8/qcolor.html#predefined-colors
				fgColor = "gray";
			}
			
			//Used by Doffen to recognize this host. If he already has it and it is connected
			//and we set KeepConn: true then he won't disconnect it
			var extID = Instance["Placement"]["AvailabilityZone"]+"_"+Instance["InstanceId"];			
			var keepConn = false;
			if(stateName == "running") {
				keepConn = true;
			}
						
			var instanceId = Instance["InstanceId"];
			var imageId = Instance["ImageId"];
			
			var tooltip = "InstanceType: " + Instance["InstanceType"] + "\n";
			tooltip += "InstanceId: " + instanceId + "\n";
			tooltip += "ImageId: " + imageId + "\n";
			
			var platform = Instance["Platform"]
			
			// Define my Host in the Doffen SSH Tree. These hosts will be inserted as children in the
			// AWS Instances folder in Doffen SSH
			// Tunnelling through my SSH proxy
			// The folder is a child of my SSH Proxy Host and thus each host I 
			// define here is a port forwarding through the Proxy to the Amazon EC2 instance.
			// In Amazon I have opened for incoming SSH from the Proxy.
			// RemoteHost is the public IP of the EC2. Doffen SSH will automatically assign
			// a local forwarding port.
			// Username and Password is my ssh credentials on the Proxy. They are variables defined in Doffen SSH Tunnel.
			// CustomActions:
			// - $port : the local listening port that Doffen SSH assigned
			// - $ip : the local listening IP (localhost)
			// - $putty : the putty executable
			// - $winscp : the winscp executable
			//
			
			var dstHost = {
				"Name":tagName + " - " + stateName + " - " + runningTimeStr + " - " + privIp + " - " + Instance["InstanceType"], 
				"FgColor":fgColor,
				"RemoteHost":pubIp,
				"RemotePort":22,
				"Username":"${nx_ssh.uid}",
				"Password":"${nx_ssh.pwd}",
				"ExtID":extID,
				"KeepConn":keepConn, 
				"CustomActions":[],
				"Tooltip":tooltip
			};
			
			if(platform != "windows") {
				var sshKeyPath = "~/.ssh/ae/"+ keyName+".pem";
				var sshCmd = "$ssh -l "+tagUser+" -i "+sshKeyPath+" -p $port $ip";
				var sftpCmd = "$winscp /privatekey="+sshKeyPath+" "+tagUser+"@$ip:$port/var/log/";
				if(process.platform == "win32") {
					sshKeyPath = '"$myfolder/ae/' + keyName + '.ppk"';
					sshCmd = "$putty -l "+tagUser+" -i "+sshKeyPath+" -P $port $ip";
					sftpCmd = "$winscp /privatekey="+sshKeyPath+" "+tagUser+"@$ip:$port/var/log/";
				}
				dstHost["CustomActions"].push({
					"label":"SSH Term",
					"cmd":sshCmd
				});
				dstHost["CustomActions"].push({
					"label":"SSH FTP",
					"cmd":sftpCmd
				});
			}
			
			dstHost["CustomActions"].push({
				"label":"AWS EC2 Describe Instance",
				"cmd":"${console_exec} ${node} ${myfolder}/nodejs/aws_ec2_describe_instance_raw.js readonly "+ AWS.config.region +" "+ instanceId
			});
			dstHost["CustomActions"].push({
				"label":"AWS EC2 Describe Image",
				"cmd":"${console_exec} ${node} ${myfolder}/nodejs/aws_ec2_describe_image_raw.js readonly "+ AWS.config.region +" "+ imageId
			});
			
			if(stateName == "running") {
				dstHostsRunning.push(dstHost);
			} else {
				dstHostsNotRunning.push(dstHost);
			}
			
			counter++;
		}
		
	}
	
	dstHostsRunning.sort(dynamicSort("Name"));
	dstHostsNotRunning.sort(dynamicSort("Name"));
	
	var dstHosts = dstHostsRunning;
	dstHosts = dstHosts.concat(dstHostsNotRunning);
	
	console.log("BEGIN_DST_JSON_DATA")
	
	//At first I did this which is perfectly okay
/*	
 	var dstData = {"Hosts":dstHosts};
	console.log("%j",dstData);
*/	
	//... but changed to this just to get
	//an easier view of the data in the log window
	console.log("{ \"Hosts\":[");
	
	for(var i in dstHosts) {
		console.log("%j",dstHosts[i]);
		if(i<dstHosts.length-1) {
			console.log(",");
		}
	}
	
	console.log("]}");
	
	console.log("END_DST_JSON_DATA")
  }
});

function dynamicSort(property) {
    var sortOrder = 1;
    if(property[0] === "-") {
        sortOrder = -1;
        property = property.substr(1);
    }
    return function (a,b) {
        var result = (a[property] < b[property]) ? -1 : (a[property] > b[property]) ? 1 : 0;
        return result * sortOrder;
    }
}
~~~

**It prints the data to STDOUT**

Doffen recognizes BEGIN_DST_JSON_DATA in the stdout and the starts to parse the json datastructure until it see END_DST_JSON_DATA

~~~
BEGIN_DST_JSON_DATA{
	"Hosts" : [{
			"Name" : "AEDEVELOPJIT Adaptation Engine - running - a year",
			"FgColor" : "",
			"RemoteHost" : "54.72.***.***",
			"RemotePort" : 22,
			"Username" : "${nx_ssh.uid}",
			"Password" : "${nx_ssh.pwd}",
			"ExtID" : "eu-west-1c_i-1e8ac***",
			"KeepConn" : true,
			"CustomActions" : [{
					"label" : "SSH Term",
					"cmd" : "$putty -l ubuntu -i $myfolder/ae/key-developjit.ppk -P $port $ip"
				}, {
					"label" : "SSH FTP",
					"cmd" : "$winscp /privatekey=$myfolder/ae/key-developjit.ppk ubuntu@$ip:$port/var/log/"
				}, {
					"label" : "AWS EC2 Describe Instance",
					"cmd" : "${console_exec} ${node} ${myfolder}/nodejs/aws_ec2_describe_instance_raw.js readonly eu-west-1 i-1e8ac***"
				}, {
					"label" : "AWS EC2 Describe Image",
					"cmd" : "${console_exec} ${node} ${myfolder}/nodejs/aws_ec2_describe_image_raw.js readonly eu-west-1 ami-b152f***"
				}
			],
			"Tooltip" : "InstanceType: m4.large\nInstanceId: i-1e8ac***\nImageId: ami-b152f***\n"
		}, {
			"Name" : "AEPRODJIT_OLD Adaptation Engine - stopped - a year",
			"FgColor" : "gray",
			"RemotePort" : 22,
			"Username" : "${nx_ssh.uid}",
			"Password" : "${nx_ssh.pwd}",
			"ExtID" : "eu-west-1b_i-4f99f***",
			"KeepConn" : false,
			"CustomActions" : [{
					"label" : "SSH Term",
					"cmd" : "$putty -l ubuntu -i $myfolder/ae/key-prodjit.ppk -P $port $ip"
				}, {
					"label" : "SSH FTP",
					"cmd" : "$winscp /privatekey=$myfolder/ae/key-prodjit.ppk ubuntu@$ip:$port/var/log/"
				}, {
					"label" : "AWS EC2 Describe Instance",
					"cmd" : "${console_exec} ${node} ${myfolder}/nodejs/aws_ec2_describe_instance_raw.js readonly eu-west-1 i-4f99f***"
				}, {
					"label" : "AWS EC2 Describe Image",
					"cmd" : "${console_exec} ${node} ${myfolder}/nodejs/aws_ec2_describe_image_raw.js readonly eu-west-1 ami-30643***"
				}
			],
			"Tooltip" : "InstanceType: m4.xlarge\nInstanceId: i-4f99f***\nImageId: ami-30643***\n"
		}, {
			"Name" : "DEVELOP Adaptation Engine - stopped - a year",
			"FgColor" : "gray",
			"RemoteHost" : "54.194.***.***",
			"RemotePort" : 22,
			"Username" : "${nx_ssh.uid}",
			"Password" : "${nx_ssh.pwd}",
			"ExtID" : "eu-west-1a_i-c8d21***",
			"KeepConn" : false,
			"CustomActions" : [{
					"label" : "SSH Term",
					"cmd" : "$putty -l ubuntu -i $myfolder/ae/key-develop.ppk -P $port $ip"
				}, {
					"label" : "SSH FTP",
					"cmd" : "$winscp /privatekey=$myfolder/ae/key-develop.ppk ubuntu@$ip:$port/var/log/"
				}, {
					"label" : "AWS EC2 Describe Instance",
					"cmd" : "${console_exec} ${node} ${myfolder}/nodejs/aws_ec2_describe_instance_raw.js readonly eu-west-1 i-c8d21***"
				}, {
					"label" : "AWS EC2 Describe Image",
					"cmd" : "${console_exec} ${node} ${myfolder}/nodejs/aws_ec2_describe_image_raw.js readonly eu-west-1 ami-d29c2***"
				}
			],
			"Tooltip" : "InstanceType: m3.xlarge\nInstanceId: i-c8d21***\nImageId: ami-d29c2***\n"
		}
	]
}
END_DST_JSON_DATA
~~~


**On windows console_exec is an executable variable configured in doffen: $myfolder\console_exec.bat**

~~~
%*
pause
~~~

**On mac console_exec is an executable variable configured in doffen: /Applications/DoffenSSHTunnel.app/Contents/MacOS/iterm2win.sh closeme**

iterm2win.sh comes bundled on mac

**aws_ec2_describe_instance_raw.js**
~~~
var AWS = require('aws-sdk'); //http://docs.aws.amazon.com/AWSJavaScriptSDK/latest/AWS/EC2.html#describeInstances-property

var args = process.argv.slice(2);
if(args.length != 3) {
	console.log("Invalid arguments");
	return 1;
}

var argAwsProfile = args[0];
var argRegion = args[1];
var argInstanceId = args[2];

//AWS.config.update({accessKeyId: argAccessKeyId, secretAccessKey: argSecretAccessKey});
var credentials = new AWS.SharedIniFileCredentials({profile: argAwsProfile});
AWS.config.credentials = credentials;
AWS.config.region = argRegion;

var ec2 = new AWS.EC2({apiVersion: '2015-04-15'});

var params = {
  InstanceIds: [
    argInstanceId,
  ]
};

ec2.describeInstances(params, function(err, data) {
  if (err) {
	// an error occurred
	console.log(err, err.stack); 
	process.exit(1);
  } else {
	console.log(JSON.stringify(data, null, 4));
	return;
  }
});
~~~


**aws_ec2_describe_image_raw.js**
~~~
var AWS = require('aws-sdk'); //http://docs.aws.amazon.com/AWSJavaScriptSDK/latest/AWS/EC2.html#describeInstances-property

var args = process.argv.slice(2);
if(args.length != 3) {
	console.log("Invalid arguments");
	return 1;
}

var argAwsProfile = args[0];
var argRegion = args[1];
var argImageId = args[2];

//AWS.config.update({accessKeyId: argAccessKeyId, secretAccessKey: argSecretAccessKey});
var credentials = new AWS.SharedIniFileCredentials({profile: argAwsProfile});
AWS.config.credentials = credentials;
AWS.config.region = argRegion;

var ec2 = new AWS.EC2({apiVersion: '2015-04-15'});


var params = {
  ImageIds: [
    argImageId,
  ]
};

ec2.describeImages(params, function(err, data) {
  if (err) {
	// an error occurred
	console.log(err, err.stack); 
	process.exit(1);
  } else {
	console.log(JSON.stringify(data, null, 4));
	return;
  }
});
~~~