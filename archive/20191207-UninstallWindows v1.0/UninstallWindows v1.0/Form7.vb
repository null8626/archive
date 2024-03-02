Public Class Form7
    Private Sub Button1_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles Button1.Click
        Timer1.Start()
        Label1.Text = ("Uninstalling Software...")
        Label4.Text = ("Estimated time remaining: 5 seconds remaining")
        Label2.Text = ("Deleting: C:\Windows")
        Label3.Text = ("Windows is deleting itself.")
    End Sub
    Private Sub Timer1_Tick(sender As Object, e As EventArgs) Handles Timer1.Tick
        ProgressBar1.Increment(10)
        If ProgressBar1.Value = 100 Then
            ProgressBar1.Value = 100
            Timer1.Stop()
            Button1.Enabled = False
            Label1.Text = ("Uninstalling process done")
            Label4.Text = ("Estimated time remaining: 0 seconds remaining")
            Label2.Text = ("Windows software deleted.")
            Label3.Text = ("Windows have deleted itself.")
            MsgBox("ERROR: User blocked from deleting the windows directory due to no permission from the computer AI. Please try uninstalling this computer using the madlad way (Smashing the computer).", vbOKOnly + vbExclamation, "Unauthorised user detected deleting the windows directory without permission.")
        End If
    End Sub
End Class