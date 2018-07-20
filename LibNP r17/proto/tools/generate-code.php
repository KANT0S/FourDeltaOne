<?php
$script = new DataScript();
$script->read('messages.txt');

generate_file($script->data, 'cpp-h.tpl', '../libnp/include/private/MessageDefinition.h');
generate_file($script->data, 'cpp-c.tpl', '../libnp/src/MessageDefinition.cpp');
generate_file($script->data, 'csharp.tpl', '../server/NPServer/NP/MessageDefinition.cs');
//generate_file($script->data, 'csharp.tpl', '../../plaza/client/Plaza/NP/MessageDefinition.cs');
generate_file($script->data, 'js.tpl', '../npm/server/messageDefinition.js');

function generate_file($messages, $template, $output)
{
	ob_start();
	include($template);
	$c = ob_get_contents();
	ob_end_clean();
	
	file_put_contents($output, $c);
}

class DataScript
{
	public $data = array();
	
	public function read($file)
	{
		$text = file_get_contents($file);
		$state = 'root';
		$currentBlock = array();
		$currentKey = '';
		$currentValue = '';
		
		for ($i = 0; $i < strlen($text); $i++)
		{
			$c = $text[$i];
			
			switch ($state)
			{
				case 'root':
					if ($c == '{')
					{
						$currentBlock = array();
						$state = 'block';
					}
				break;
				case 'block':
					if ($c == '}')
					{
						$this->data[] = $currentBlock;
						$state = 'root';
					}
					else if ($c == '"')
					{
						if (empty($currentKey))
						{
							$state = 'key';
						}
						else
						{
							$state = 'value';
						}
					}
					else if ($c == "\n")
					{
						$currentKey = '';
						$currentValue = '';
					}
				break;
				case 'key':
				case 'value':
					if ($c == '"')
					{
						if ($state == 'value')
						{
							$currentBlock[$currentKey] = $currentValue;
						}
					
						$state = 'block';
					}
					else
					{
						if ($state == 'key')
						{
							$currentKey .= $c;
						}
						else
						{
							$currentValue .= $c;
						}
					}
				break;
			}
		}
		
		assert($state == 'root');
	}
	
	public function write()
	{
		$output = '';
	
		foreach ($this->data as $entity)
		{
			$output .= sprintf("{\n");
			
			foreach ($entity as $key => $value)
			{
				$output .= sprintf("\t\"%s\" \"%s\"\n", $key, $value);
			}
			
			$output .= sprintf("}\n");
		}
		
		return $output;
	}
}