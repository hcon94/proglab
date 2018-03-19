package JavafxEx;
import javafx.application.Application;
import javafx.fxml.FXMLLoader;
import javafx.scene.Parent;
import javafx.scene.Scene;
import javafx.stage.Stage;


public class RemoteConApp extends Application{
	 @Override
	    public void start(Stage primaryStage) throws Exception{
	        Parent root = FXMLLoader.load(getClass().getResource("RemoteController.fxml"));
	        primaryStage.setTitle("Here you can control the destructoBot");
	        primaryStage.setScene(new Scene(root, 800, 500));
	        primaryStage.show();
	    }


	    public static void main(String[] args) {
	        launch(args);
	    }
	}

