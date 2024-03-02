Public Class Form6
    Private Sub Button1_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles Button1.Click
        Timer1.Start()
        Label1.Text = ("Uninstalling Hardware...")
        Label4.Text = ("Estimated time remaining: 20 minutes remaining")
        Label2.Text = ("Deleting: C:\Windows\System32\Hardware.exe")
        Label3.Text = ("Windows is deleting itself.")
        Label5.Text = ("Please wait until the process is done...")
        Label6.Text = ("...And the screen turns black.")
    End Sub
    Private Sub Timer1_Tick(sender As Object, e As EventArgs) Handles Timer1.Tick
        ProgressBar1.Increment(10)
        If ProgressBar1.Value = 98 Then
            ProgressBar1.Value = 98
            Timer1.Stop()
            Button1.Enabled = False
            Label1.Text = ("Uninstalling Cancelled")
            Label4.Text = ("Estimated time remaining: 0 seconds remaining")
            Label2.Text = ("ERROR: Unable to delete Hardware.exe.")
            Label3.Text = ("Windows seem to be unable to delete itself.")
            Label5.Text = ("Maybe it is because of wrong space time")
            Label6.Text = ("Or it is because of you")
            MsgBox("Error: Cannot delete hardware. Problem is caused probably because of incorrect space time/universe.", vbOKOnly + vbInformation, "Uninstall Error")
        End If
    End Sub


End Class