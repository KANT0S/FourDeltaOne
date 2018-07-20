/*
SQLyog Community v11.31 (64 bit)
MySQL - 5.5.24-log : Database - xnp
*********************************************************************
*/

/*!40101 SET NAMES utf8 */;

/*!40101 SET SQL_MODE=''*/;

/*!40014 SET @OLD_UNIQUE_CHECKS=@@UNIQUE_CHECKS, UNIQUE_CHECKS=0 */;
/*!40014 SET @OLD_FOREIGN_KEY_CHECKS=@@FOREIGN_KEY_CHECKS, FOREIGN_KEY_CHECKS=0 */;
/*!40101 SET @OLD_SQL_MODE=@@SQL_MODE, SQL_MODE='NO_AUTO_VALUE_ON_ZERO' */;
/*!40111 SET @OLD_SQL_NOTES=@@SQL_NOTES, SQL_NOTES=0 */;
CREATE DATABASE /*!32312 IF NOT EXISTS*/`xnp` /*!40100 DEFAULT CHARACTER SET latin1 */;

USE `xnp`;

/*Table structure for table `ban_list` */

DROP TABLE IF EXISTS `ban_list`;

CREATE TABLE `ban_list` (
  `username` varchar(255) NOT NULL,
  `reason` varchar(255) NOT NULL,
  `admin_username` varchar(255) NOT NULL
) ENGINE=InnoDB DEFAULT CHARSET=latin1;

/*Data for the table `ban_list` */

/*Table structure for table `external_platforms` */

DROP TABLE IF EXISTS `external_platforms`;

CREATE TABLE `external_platforms` (
  `Platform_ID` int(11) NOT NULL,
  `Platform_Type` int(11) NOT NULL,
  `User_ID` int(11) NOT NULL,
  `platform_authenticated` int(12) NOT NULL
) ENGINE=MyISAM DEFAULT CHARSET=latin1;

/*Data for the table `external_platforms` */

insert  into `external_platforms`(`Platform_ID`,`Platform_Type`,`User_ID`,`platform_authenticated`) values (2147483647,0,0,0),(2147483647,0,1,0),(2147483647,0,101259,0),(1,1,1,0),(2147483647,0,0,0),(2147483647,0,1,0),(2147483647,0,101259,0),(1,1,1,0),(2147483647,0,0,0),(2147483647,0,1,0),(2147483647,0,101259,0),(1,1,1,0),(2147483647,0,0,0),(2147483647,0,1,0),(2147483647,0,101259,0),(1,1,1,0);

/*Table structure for table `externalplatforms` */

DROP TABLE IF EXISTS `externalplatforms`;

CREATE TABLE `externalplatforms` (
  `UserID` int(11) NOT NULL,
  `platformauthenticated` int(12) NOT NULL,
  `PlatformID` int(11) NOT NULL
) ENGINE=MyISAM DEFAULT CHARSET=latin1;

/*Data for the table `externalplatforms` */

/*Table structure for table `friend` */

DROP TABLE IF EXISTS `friend`;

CREATE TABLE `friend` (
  `friend` tinyint(1) NOT NULL,
  `friend_id` int(10) NOT NULL,
  `is_external` tinyint(1) NOT NULL,
  `user_id` int(10) NOT NULL
) ENGINE=MyISAM DEFAULT CHARSET=latin1;

/*Data for the table `friend` */

/*Table structure for table `friends` */

DROP TABLE IF EXISTS `friends`;

CREATE TABLE `friends` (
  `friend` int(11) NOT NULL,
  `friend_id` int(11) NOT NULL,
  `is_external` int(11) NOT NULL,
  `user_id` int(11) NOT NULL
) ENGINE=MyISAM DEFAULT CHARSET=latin1;

/*Data for the table `friends` */

/*Table structure for table `iw4_profiles` */

DROP TABLE IF EXISTS `iw4_profiles`;

CREATE TABLE `iw4_profiles` (
  `user_ID` int(11) NOT NULL,
  `Prestige` tinyint(4) NOT NULL,
  `experience` int(11) NOT NULL,
  `cardicon` varchar(60) NOT NULL,
  `cardtitle` varchar(60) NOT NULL
) ENGINE=MyISAM DEFAULT CHARSET=latin1;

/*Data for the table `iw4_profiles` */

/*Table structure for table `licensekeys` */

DROP TABLE IF EXISTS `licensekeys`;

CREATE TABLE `licensekeys` (
  `id` int(11) NOT NULL,
  `key` varchar(24) CHARACTER SET utf8 COLLATE utf8_bin NOT NULL,
  `hashkey` varchar(48) CHARACTER SET utf8 COLLATE utf8_bin NOT NULL,
  `shortKey` varchar(16) CHARACTER SET utf8 COLLATE utf8_bin NOT NULL,
  `type` int(1) NOT NULL
) ENGINE=MyISAM DEFAULT CHARSET=latin1;

/*Data for the table `licensekeys` */

insert  into `licensekeys`(`id`,`key`,`hashkey`,`shortKey`,`type`) values (1,'nAmQCs6RtdLCG0NkvE7O2Re8','','',1),(1,'nAmQCs6RtdLCG0NkvE7O2Re8','','',1),(1,'nAmQCs6RtdLCG0NkvE7O2Re8','','',1),(1,'nAmQCs6RtdLCG0NkvE7O2Re8','','',1);

/*Table structure for table `match_users` */

DROP TABLE IF EXISTS `match_users`;

CREATE TABLE `match_users` (
  `match_id` char(40) NOT NULL,
  `match_user` int(32) NOT NULL
) ENGINE=MyISAM DEFAULT CHARSET=latin1;

/*Data for the table `match_users` */

/*Table structure for table `matches` */

DROP TABLE IF EXISTS `matches`;

CREATE TABLE `matches` (
  `match_filename` varchar(255) NOT NULL,
  `match_gametype` char(10) NOT NULL,
  `match_guid` char(40) NOT NULL,
  `match_map` varchar(40) NOT NULL,
  `match_time` int(32) NOT NULL
) ENGINE=MyISAM DEFAULT CHARSET=latin1;

/*Data for the table `matches` */

/*Table structure for table `pw_ask` */

DROP TABLE IF EXISTS `pw_ask`;

CREATE TABLE `pw_ask` (
  `email` varchar(255) NOT NULL,
  `code` int(11) NOT NULL
) ENGINE=InnoDB DEFAULT CHARSET=latin1;

/*Data for the table `pw_ask` */

/*Table structure for table `users` */

DROP TABLE IF EXISTS `users`;

CREATE TABLE `users` (
  `id` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `username` varchar(48) CHARACTER SET utf8 NOT NULL,
  `password` varchar(48) CHARACTER SET utf8 NOT NULL,
  `email` varchar(255) CHARACTER SET utf8 NOT NULL,
  `sid` varchar(32) NOT NULL,
  `admin` int(1) NOT NULL,
  `hwid` varchar(255) NOT NULL,
  PRIMARY KEY (`id`)
) ENGINE=MyISAM AUTO_INCREMENT=1015 DEFAULT CHARSET=latin1;

/*Data for the table `users` */

insert  into `users`(`id`,`username`,`password`,`email`,`sid`,`admin`,`hwid`) values (1012,'Fl0w','cb682f9503b5a831101cd88d5b89bde805e4e6ac','lel@lel.lel','',1,''),(1013,'DontShoot','878b0f84ae2bc2329f1d06667fde6bb57a4b35cd','d3ad-r3d@hotmail.com','',0,''),(1014,'Solid','fd922e219a75972903e80c2190eeb0de81f956fc','ud.solid@yahoo.com','',1,'');

/*!40101 SET SQL_MODE=@OLD_SQL_MODE */;
/*!40014 SET FOREIGN_KEY_CHECKS=@OLD_FOREIGN_KEY_CHECKS */;
/*!40014 SET UNIQUE_CHECKS=@OLD_UNIQUE_CHECKS */;
/*!40111 SET SQL_NOTES=@OLD_SQL_NOTES */;
