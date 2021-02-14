<?
// This file is part of ssh_mgr2
// Copyright (C) 2006 Gabriel Schulhof <nix@go-nix.ca>
// Package ssh_mgr2 released under GPL (http://www.gnu.org/licenses/gpl.txt):
//
//  This program is free software; you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation; either version 2 of the License, or
//  (at your option) any later version.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License along
//  with this program; if not, write to the Free Software Foundation, Inc.,
//  51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.  

header("Cache-Control: no-cache, must-revalidate"); // HTTP/1.1
header("Expires: Mon, 26 Jul 1997 05:00:00 GMT"); // Date in the past

$clrs = array (
    // pure colours [0]
    "#000000", "#800000", "#008000", "#808000",
    "#000080", "#808000", "#008080", "#C0C0C0",
    "#808080", "#FF0000", "#00FF00", "#FFFF00",
    "#0000FF", "#FF00FF", "#00FFFF", "#FFFFFF",
    // cursor colour [16]
    "#A0C040",
    // 50% medium shade boxes [17]
    "#000000", "#400000", "#004000", "#404000", "#000040", "#404000", "#004040", "#606060", "#404040", "#7f0000", "#007f00", "#7f7f00", "#00007f", "#7f007f", "#007f7f", "#7f7f7f",
    "#800000", "#404000", "#804000", "#400040", "#804000", "#404040", "#a06060", "#804040", "#bf0000", "#407f00", "#bf7f00", "#40007f", "#bf007f", "#407f7f", "#bf7f7f",
    "#008000", "#408000", "#004040", "#408000", "#008040", "#60a060", "#408040", "#7f4000", "#00bf00", "#7fbf00", "#00407f", "#7f407f", "#00bf7f", "#7fbf7f",
    "#808000", "#404040", "#808000", "#408040", "#a0a060", "#808040", "#bf4000", "#40bf00", "#bfbf00", "#40407f", "#bf407f", "#40bf7f", "#bfbf7f",
    "#000080", "#404040", "#004080", "#6060a0", "#404080", "#7f0040", "#007f40", "#7f7f40", "#0000bf", "#7f00bf", "#007fbf", "#7f7fbf",
    "#808000", "#408040", "#a0a060", "#808040", "#bf4000", "#40bf00", "#bfbf00", "#40407f", "#bf407f", "#40bf7f", "#bfbf7f",
    "#008080", "#60a0a0", "#408080", "#7f4040", "#00bf40", "#7fbf40", "#0040bf", "#7f40bf", "#00bfbf", "#7fbfbf",
    "#c0c0c0", "#a0a0a0", "#df6060", "#60df60", "#dfdf60", "#6060df", "#df60df", "#60dfdf", "#dfdfdf",
    "#808080", "#bf4040", "#40bf40", "#bfbf40", "#4040bf", "#bf40bf", "#40bfbf", "#bfbfbf",
    "#ff0000", "#7f7f00", "#ff7f00", "#7f007f", "#ff007f", "#7f7f7f", "#ff7f7f",
    "#00ff00", "#7fff00", "#007f7f", "#7f7f7f", "#00ff7f", "#7fff7f",
    "#ffff00", "#7f7f7f", "#ff7f7f", "#7fff7f", "#ffff7f",
    "#0000ff", "#7f00ff", "#007fff", "#7f7fff",
    "#ff00ff", "#7f7fff", "#ff7fff",
    "#00ffff", "#7fffff",
    "#ffffff") ;

define ("FORM_ACTION", 
  ($HTTP_SERVER_VARS["HTTPS"] == "on" ? "https" : "http") . "://" . 
  ((isset ($_SERVER["HTTP_HOST"]) || "" != $_SERVER["HTTP_HOST"]) ? 
    $_SERVER["HTTP_HOST"] : 
    $_SERVER["SERVER_NAME"]) . 
  $_SERVER["PHP_SELF"]) ;

$session_id  = $_POST['session_id'] ;
$host_name   = $_POST['host_name'] ;
$user_name   = $_POST['user_name'] ;
$form_action = $_POST['submit_button'] ;
$rows        = $_POST['rows'] ;
$cols        = $_POST['cols'] ;
$input_data_text     = $_POST['input_data_text'] ;
$input_data_password = $_POST['input_data_password'] ;
$input_data_script   = $_POST['input_data_script'] ;
$input_data_code     = $_POST['input_data_code'] ;

$input = "" ;
$input_length = 0 ;

function interpret_input_code ($str)
    {
    $bESC = false ;
    $bInField = false ;
    $txt = "" ;
    $stroke = "" ;

    for ($Nix = 0 ; $Nix < strlen ($str) ; $Nix++)
        {
        if ($bESC && $ic >= 0 && $bInField)
            {
            $stroke .= substr ($str, $Nix, 1) ;
            $bESC = false ;
            continue ;
            }
            
        if ("{" == substr ($str, $Nix, 1) && !$bESC)
            {
            if ($bInField)
                return "" ;
            $bInField = true ;
            $stroke = "" ;
            continue ;
            }
            
        if ("}" == substr ($str, $Nix, 1) && !$bESC)
            {
            if (!$bInField)
                return "" ;
            $bInField = false ;
            $txt .= InterpretJavaStroke ($stroke) ;
            continue ;
            }
            
        if ("\\" == substr ($str, $Nix, 1))
            {
            $bESC = true ;
            continue ;
            }
        
        if ($bInField)
            $stroke .= substr ($str, $Nix, 1) ;
        }
    return $txt ;
    }

function InterpretJavaStroke ($str)
    {
    /* Some key escapes */
    $arKeys = array (
/*   Key Description              Ctrl+Key                           Key                 */
         "F1"       => array ("true" => chr (27) . "[O5P"  , "false" => chr (27) . "[OP" ),
         "F2"       => array ("true" => chr (27) . "[O5Q"  , "false" => chr (27) . "[OQ" ),
         "F3"       => array ("true" => chr (27) . "[O5R"  , "false" => chr (27) . "[OR" ),
         "F4"       => array ("true" => chr (27) . "[O5S"  , "false" => chr (27) . "[OS" ),
         "F5"       => array ("true" => chr (27) . "[15;5~", "false" => chr (27) . "[15~"),
         "F6"       => array ("true" => chr (27) . "[17;5~", "false" => chr (27) . "[17~"),
         "F7"       => array ("true" => chr (27) . "[18;5~", "false" => chr (27) . "[18~"),
         "F8"       => array ("true" => chr (27) . "[19;5~", "false" => chr (27) . "[19~"),
         "F9"       => array ("true" => chr (27) . "[20;5~", "false" => chr (27) . "[20~"),
        "F10"       => array ("true" => chr (27) . "[21;5~", "false" => chr (27) . "[21~"),
        "F11"       => array ("true" => chr (27) . "[23;5~", "false" => chr (27) . "[23~"),
        "F12"       => array ("true" => chr (27) . "[24;5~", "false" => chr (27) . "[24~"),
        "Up"        => array ("true" => chr (27) . "[5A"   , "false" => chr (27) . "OA" ),
        "Down"      => array ("true" => chr (27) . "[5B"   , "false" => chr (27) . "OB" ),
        "Right"     => array ("true" => chr (27) . "[5C"   , "false" => chr (27) . "OC" ),
        "Left"      => array ("true" => chr (27) . "[5D"   , "false" => chr (27) . "OD" ),
        "Insert"    => array ("true" => chr (27) . "[2;5~" , "false" => chr (27) . "[2~" ),
        "Delete"    => array ("true" => chr (27) . "[3~"   , "false" => chr (27) . "[3~" ),
        "Home"      => array ("true" => chr (27) . "[1~"   , "false" => chr (27) . "[1~" ),
        "End"       => array ("true" => chr (27) . "[4~"   , "false" => chr (27) . "[4~" ),
        "Page Up"   => array ("true" => chr (27) . "[5;5~" , "false" => chr (27) . "[5~" ),
        "Page Down" => array ("true" => chr (27) . "[6;5~" , "false" => chr (27) . "[6~" ),
        "Escape"    => array ("true" => chr (27)           , "false" => chr (27)         ),
        "Enter"     => array ("true" => chr (13)           , "false" => chr (13)         ),
        ) ;
    
    $txt = "" ;
    $res = "" ;
    
    $bCtrl = false ;
    $bAlt = false ;
    
    $ar = explode ("+", $str) ;
    
    /* Deal with regular text */
    if ("text" == strtolower ($ar[0]))
        {
        for ($Nix = 1 ; $Nix < count ($ar) ; $Nix++)
            $txt .= $ar[$Nix] . "+" ;
        $txt = substr ($txt, 0, strlen ($txt) - 1) ;
        return $txt ;
        }
    
    /* Otherwise, first grab all modifiers that we understand */
    for ($Nix = 0 ; $Nix < count ($ar) ; $Nix++)
        {
        if ("ctrl" == strtolower ($ar[$Nix]))
            $bCtrl = true ;
        else if ("alt" == strtolower ($ar[$Nix]))
            $bAlt = true ;
        else
            break ;
        }
    
    /* Then, grab the actual text */
    for (; $Nix < count ($ar) ; $Nix++)
        $txt .= $ar[$Nix] . "+" ;
    $txt = substr ($txt, 0, strlen ($txt) - 1) ;

    /* Then, modify it appropriately */
    if (isset ($arKeys[$txt]))
        return $arKeys[$txt][$bCtrl ? "true" : "false"] ;
    /* Enter key */
    else if ("enter" == strtolower ($txt))
        $txt = ($bAlt ? chr (27) : "") . "\n" ;
    /* Tab */
    else if ("tab" == strtolower ($txt))
        $txt = ($bAlt ? chr (27) : "") . chr (9) ;
    /* Backspace */
    else if ("backspace" == strtolower ($txt))
        $txt = ($bAlt ? chr (27) : "") . chr (8) ;
    else if ("caps lock" == strtolower ($txt)) /* ignore CAPS LOCK */
        return "" ;
    /* The rest (most likely single chars) */
    else
        {
        $ret = "" ;
        for ($Nix = 0 ; $Nix < strlen ($txt) ; $Nix++)
            $ret .= 
                ($bAlt ? chr (27) : "") . 
                ($bCtrl ? chr (ord (strtoupper (substr ($txt, $Nix, 1))) - 64) : $txt) ;
        $txt = $ret ;
        }
    return $txt ;
    }

function remove_lf_and_escapes ($str)
	{
	$strRet = "" ;
	for ($Nix = 0 ; $Nix < strlen ($str) ; $Nix++)
		if ("\n" != $str[$Nix])
			{
			if ("\\" == $str[$Nix])
				$Nix++ ;
			$strRet .= $str[$Nix] ;
			}
	return $strRet ;
	}

function determine_input (&$input_length, $input_data_text, $input_data_password, $input_data_script, $input_data_code)
  {
  $ret = "" ;
  $input_length = 0 ;

  if (strcmp ($input_data_password, ""))
    $input_length = strlen ($ret = remove_lf_and_escapes ($input_data_password) . "\n") ;
  else
  if (strcmp ($input_data_text, ""))
    $input_length = strlen ($ret = remove_lf_and_escapes ($input_data_text) . "\n") ;
  else
  if (strcmp ($input_data_script, ""))
    $input_length = strlen ($ret = remove_lf_and_escapes ($input_data_script)) ;
  else
  if (strcmp ($input_data_code, ""))
    $input_length = strlen ($ret = interpret_input_code ($input_data_code)) ;

  return $ret ;
  }

function fwrite_or_close ($the_socket, $string)
  {
  if (!($ret = fwrite ($the_socket, $string)))
//    {
//    echo ("<!-- fwrite_or_close: dying -->\n") ;
    fclose ($the_socket) ;
//    }
  return $ret ;
  }

function read_line_or_close ($the_socket)
  {
  $ret = "" ;

  if (!($ret = fgets ($the_socket, 1024)))
    {
//    echo ("<!-- read_line_or_close: dying before loop -->\n") ;
    fclose ($the_socket) ;
    return FALSE ;
    }

  while (strcmp (substr ($ret, -1), "\n"))
    if (!($str = fgets ($the_socket, 1024)))
      {
//      echo ("<!-- read_line_or_close: dying during loop -->\n") ;
      fclose ($the_socket) ;
      return FALSE ;
      }
    else
      $ret .= $str ;

  return substr ($ret, 0, -1) ;
  }

function read_status ($str)
  {
  if (!$str) return FALSE ;
  if (strcmp (substr ($str, 0, 8), "<status>")) return FALSE ;
  if (strcmp (substr ($str, -9), "</status>")) return FALSE ;
  return substr ($str, 8, -9) ;
  }

function print_screen ($session_id, $host_name, $user_name, $rows, $cols, $input, $input_length)
  {
//  echo ("\n<!-- print_screen: Entering -->\n") ;
  if ("" == $session_id) return FALSE ;

  if (!($the_socket = fsockopen ("unix:///tmp/ssh_mgr", 0))) return FALSE ;

  if (!fwrite_or_close ($the_socket, "<key>$session_id</key>")) return FALSE ;

//  echo ("<!-- print_screen: Calling read_line_or_close to read the status after the key send -->\n") ;
  if (!($status = read_status (read_line_or_close ($the_socket)))) return FALSE ;
//  echo ("<!-- print_screen: Called read_line_or_close to read the status after the key send -->\n") ;

  if ("New" == $status)
    {
    if (!fwrite_or_close ($the_socket, "<user_name>$user_name</user_name>")) return FALSE ;
    if (!fwrite_or_close ($the_socket, "<host_name>$host_name</host_name>")) return FALSE ;
    if (!fwrite_or_close ($the_socket, "<rows>$rows</rows>")) return FALSE ;
    if (!fwrite_or_close ($the_socket, "<cols>$cols</cols>")) return FALSE ;
    }

  if ("Dead" == $status)
    {
//    echo ("<!-- Status: Dead -->\n") ;
    fclose ($the_socket) ;
    return FALSE ;
    }

  if (!fwrite_or_close ($the_socket, "<input_length>" . $input_length . "</input_length>")) return FALSE ;

  if ($input_length > 0)
    if (!fwrite ($the_socket, $input, $input_length))
      {
//      echo ("<!-- Failed to write input length -->\n") ;
      fclose ($the_socket) ;
      return FALSE ;
      }

echo (
"  <tr>
    <td align=\"center\" valign=\"middle\" colspan=\"3\">
      <table border=\"1\" cellspacing=\"0\" cellpadding=\"0\" bgcolor=\"#44b1eb\">
        <tr>
          <td>\n") ;

  echo ("<table border=\"0\" cellspacing=\"0\" cellpadding=\"0\">\n") ;
//  echo ("<!-- print_screen: Calling read_line_or_close for first line of screen -->\n") ;
  if (($line = read_line_or_close ($the_socket)))
    {
//    echo ("<!-- print_screen: Called read_line_or_close for first line of screen -->\n") ;
    create_title_row ($line) ;
    while (($line = read_line_or_close ($the_socket)))
      if (($status = read_status ($line)))
        break ;
      else
        create_row ($line) ;
    }
  echo ("</table>\n") ;

echo (
"          </td>
        </tr>
      </table>
    </td>
  </tr>\n") ;

  fclose ($the_socket) ;
//  echo ("\n<!-- Exiting print_screen -->\n") ;
  return TRUE ;
  }

function create_row ($line)
  {
  echo ("<tr>") ;
  while ($line)
    {
    $beg_cell = strpos ($line, '{') ;
    $end_cell = strpos ($line, '}') ;
    create_cell (substr ($line, $beg_cell, $beg_cell + $end_cell + 1)) ;
    $line = substr ($line, $end_cell + 1) ;
    }
  echo ("</tr>\n") ;
  }

function create_title_row ($cell)
  {
  $strlen_title = strlen ($title = substr ($cell = read_cell_attrs ($cell, $cols, $bg_clr), 0, -1)) ;
  $count1 = ($cols - $strlen_title) >> 1 ;
  $count2 = $cols - $count1 - $strlen_title ;

  echo ("<tr>") ;

  for ($Nix = 0 ; $Nix < $count1 ; $Nix++)
    echo ("<td><tt>&nbsp;</tt></td>") ;

  for ($Nix = 0 ; $Nix < $strlen_title ; $Nix++)
    echo ("<td><tt>" . (' ' == $title{$Nix} ? "&nbsp;" : $title{$Nix}) . "</tt></td>") ;

  for ($Nix = 0 ; $Nix < $count2 ; $Nix++)
    echo ("<td><tt>&nbsp;</tt></td>") ;

  echo ("</tr>\n") ;
  }

function read_cell_attrs ($cell, &$cols, &$bg_clr)
  {
  global $clrs ;

  $cell = substr ($cell, 1) ;

  $idx = strpos ($cell, ':') ;
  $cols = substr ($cell, 0, $idx) ;  
  $cell = substr ($cell, $idx + 1) ;

  $idx = strpos ($cell, ':') ;
  $bg_clr = $clrs[substr ($cell, 0, $idx)] ;

  return substr ($cell, $idx + 1) ;
  }

function create_cell ($cell)
  {
  global $clrs ;

  $cell = read_cell_attrs ($cell, $cols, $bg_clr) ;

  echo ("<td align=\"left\" bgcolor=\"$bg_clr\" colspan=\"$cols\"><tt>") ;

  while ('}' != $cell{0})
    {
    if ('<' == $cell{0})
      $cell = do_tag ($cell) ;
    else
      {
      if (32 == ($val = hexdec ($cell{0} . $cell{1})))
        echo ("&nbsp;") ;
      else
        echo (htmlentities ((string)chr ($val))) ;
      $cell = substr ($cell, 2) ;
      }
    }

  echo ("</tt></td>") ;
  }

function do_tag ($str)
  {
  global $clrs ;

  $tags = array ("c" => "font", "b" => "b", "u" => "u", "l" => "blink") ;

  $idx = strpos ($str, '>') ;
  if ('/' == $str{1})
    echo ("</" . $tags[$str{2}] . ">") ;
  else
  if ('c' == $str{1})
    echo ("<font color=\"" . $clrs[substr ($str, 2, $idx - 2)] . "\">") ;
  else
    echo ("<" . $tags[$str{1}] . ">") ;

  return substr ($str, $idx + 1) ;
  }

if ("Connect" == $form_action)
  $session_id = md5 (uniqid (rand (), 1)) ;

if ("Send" == $form_action)
  $input = determine_input ($input_length, $input_data_text, $input_data_password, $input_data_script, $input_data_code) ;
?>
<html>
<head>
<meta http-equiv="Content-Type" content="text/html; charset=utf-8">
<title>Nix's&nbsp;Web-based&nbsp;SSH&nbsp;Client</title>
<link rel="stylesheet" href="style.css">
<script type="text/javascript">
var formInUse = false ;

function setFocus ()
  {
  if (!formInUse)
    {
    document.input.input_data_text.focus () ;
    }
  }
</script>
</head>
<body bgcolor="#000000" onload="setFocus()" leftmargin="0" topmargin="0"><!--<center>-->
<table border="0" cellspacing="0" cellpadding="0" width="100%" height="100%"><tr><td align="center" valign="middle">
<table border="0" cellspacing="0" cellpadding="0" bgcolor="#ffffff">
  <tr>
    <td align="left" valign="top"><img src="top_left_corner.png"></td>
    <td><h1>Nix's Web-based SSH Client</h1></td>
    <td align="right" valign="top"><img src="top_right_corner.png"></td>
  </tr>
<?  if (print_screen ($session_id, $host_name, $user_name, $rows, $cols, $input, $input_length)) {?>
<!-- Input form -->
  <tr>
    <td valign="bottom" align="left"><img src="bottom_left_corner.png"></td>
    <td align="center" valign="top">
      <table border="0" cellspacing="3" cellpadding="0">
      <tr><td>
      <form name="input" method="post" action="<?=FORM_ACTION;?>">
      <input type="hidden" name="session_id" value="<?=$session_id;?>">
      <table border="0" cellspacing="2" cellpadding="2">
        <tr><td align="left" valign="center">
        <table border="0" cellspacing="2" cellpadding="0">
          <tr>
            <td align="right" valign="center">Command:</td>
            <td><input type="text" name="input_data_text" size="50"></td>
          </tr>
          <tr>
            <td align="right" valign="center">Password:</td>
            <td><input type="password" name="input_data_password" size="13"></td>
          </tr>
        </table>
        </td></tr>
        <tr>
          <td align="left" valign="middle">Script:</td>
        </tr>
        <tr>
          <td align="left" valign="middle">
            <textarea name="input_data_script" cols="80" rows="4"></textarea>
          </td>
        </tr>
        <tr>
          <td align="left" valign="middle">Code: <font color="#808080">{text+Blah}{Ctrl+Z}{F1}{PageUp}{Enter} etc.</font></td>
        </tr>
        <tr>
          <td align="left" valign="middle">
            <textarea name="input_data_code" cols="80" rows="4"></textarea>
          </td>
        </tr>
        <tr>
          <td align="right" valign="center">
            <input type="submit" name="submit_button" value="Send">
          </td>
        </tr>
      </table>
      </form>
      </td></tr>
      </table>
    </td>
    <td valign="bottom" align="right"><img src="bottom_right_corner.png"></td>
  </tr>
<!-- End Input form -->
<? } else { ?>
<!-- Destination form -->
  <tr>
    <td valign="bottom" align="left"><img src="bottom_left_corner.png"></td>
    <td>
      <form name="destination" method="post" action="<?=FORM_ACTION;?>">
        <table border="0" cellspacing="2" cellpadding="2">
          <tr>
            <td align="right" valign="middle">Hostname:</td>
            <td align="left"  valign="middle"><input type="text"     name="host_name" size="13" value=""></td>
          </tr>
          <tr>
            <td align="right" valign="middle">Username:</td>
            <td align="left"  valign="middle"><input type="text"     name="user_name" size="13" value=""></td>
          </tr>
          <tr>
            <td align="right" valign="middle">Rows:</td>
            <td align="left"  valign="middle"><input type="text"     name="rows"      size="3"  value="25" maxlength="3"></td>
          </tr>
          <tr>
            <td align="right" valign="middle">Columns:</td>
            <td align="left"  valign="middle"><input type="text"     name="cols"      size="3"  value="80" maxlength="3"></td>
          </tr>
          <tr>
            <td colspan="2" align="right" valign="middle"><input type="submit" name="submit_button" value="Connect"></td>
          </tr>
        </table>
      </form>
    </td>
    <td valign="bottom" align="right"><img src="bottom_right_corner.png"></td>
  </tr>
<!-- End Destination form -->
<?}?>
</table>
</td></tr></table>
<!--</center>--></body>
</html>
