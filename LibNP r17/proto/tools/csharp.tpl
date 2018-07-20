// This file has been automatically generated.

using NPx;
using System;

namespace NPx
{
<?php
foreach ($messages as $message)
{
?>
	public partial class <?php echo $message['name'] ?> : NPRPCMessage<<?php echo $message['message'] ?>>
	{
		public override int Type
		{
			get
			{
				return <?php echo intval($message['type']) ?>;
			}
		}
	}
<?php
}
?>

	public class NPMessageFactory
	{
		public static INPRPCMessage CreateMessage(int type)
		{
			switch (type)
			{
<?php
foreach ($messages as $message)
{
?>
				case <?php echo intval($message['type']); ?>:
					return new <?php echo $message['name'] ?>();
<?php
}
?>
			}
			
			throw new ArgumentException(string.Format("No message handler for type {0} is defined.", type));
		}
		
		public static INPRPCMessage CreateMessage(Type type)
		{
			switch (type.Name)
			{
<?php
foreach ($messages as $message)
{
?>
				case "<?php echo $message['message']; ?>":
					return new <?php echo $message['name'] ?>();
<?php
}
?>
			}
			
			throw new ArgumentException(string.Format("No message handler for type {0} is defined.", type.Name));
		}
	}
}