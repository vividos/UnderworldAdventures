<?php
   $image = $_GET['image'];
   $text = $_GET['text'];
?>
<p align="center">
<img src="<?php echo $image; ?>" alt="<?php echo $text; ?>"><br>
<?php echo $text; ?>
</p>
