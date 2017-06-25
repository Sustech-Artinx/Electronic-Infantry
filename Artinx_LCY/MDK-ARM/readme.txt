2016年12月6日20:56:48
BUG list
1，云台开环模式， 左右有时候会有噪声，体验不好(发现是一个bug，已经修复还有就是机械原因。
2，需要增加失控保护机制，注意当云台释放后与底盘存在误差导致底盘一直疯转，目前是底盘角度超过一定值（大约50°）就会底盘保护。
XXXX：经过调参数， 把D项去掉后，发现云台噪声大大减小了！！！
3，有时间加上前馈控制，这样可以提高响应速度！
4，把初始化的一些函数放到一个 init task 里面， 全部初始化后再创建其他任务 然后删除自己。
5, maybe add filter for angular rate or position angle will better
6, new windstorm is comming ! how can we stop moving forward?! gimbal have noise!!!
7, add imu temperature control done,keep in 40'C--2016-12-13 10:36:22
8: TODO: add imu keep pit angle stable
9: TODO: limit yaw gyro mode target.2016-12-14 21:23:30 DONE！

2016-12-19 11:31:51 problems：
10:云台yaw 超过一定角度就保护， 可以把这个保护角度再加大 接近90°再保护。	//done  see item 17.
11:全部电机和模块都监控，module offline、timeout， stop the infantry。		//done
12.poke moto sometimes run bug。when enter stop mode， every things should stop!!! //done 
13.change operation compatible with old， hide yaw encoder mode。			//maybe next time
14 when fric wheel run, gimbal unstable shake.//solved!turn fric pwm -> 1400
15 if fric run faster, gyro will have too much noise. turn fric pwm -> 1400
//change to old operation mode, git commit !!! remember!!! Every Time you want make big change remenber git commit!!!
16, found a SKY BIG BUG!!!. when i filter gimbal angle. if last = 8191  now = 1  avg will be a error!!!  //FIXED!!!
next time, when you filter something, you need to make sure it is continues!!!
17. fix fast yaw target enter protect mode(chassis stop because the angle between gimbal yaw is too large!);	//done 
18. 2016-12-22 16:51:36 maybe it's time to release?
19.2016-12-23 maybe a never calied infantry will cause dangerous problem！！！?=0x55

2017-1-9 22:39:08
fix some bug。
add more comment

TODO:::

1>. write code document . time: 1 week	// DONE so far.
2>. poke can't move bug.				//DONE
3>. before 2017-1-1, have a stable version to release!!!also need a tool to calibrate gimbal+imu
4>. need add auto shoot big buff.		//DONE
  4.1>add buff cali part, wait more
  4.2>add a block to cali camera.
  4.3>big buff done!!! wait sort 
  4.4>big buff shot done! 
  4.5>ps. wait to add more protect code when cali 5+9
  